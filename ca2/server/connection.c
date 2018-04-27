#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "macros.h"

pthread_mutex_t lock_x;

void *connection(void *arg) {
    int socket = * (int*) arg;

    int authenticated = 0;
    int READSIZE;
    int blockSize;
    int fileSize = 0;

    char message[MAX_BUF];
    char line[MAX_BUF];
    char dirname[MAX_BUF];
    char path[MAX_BUF];
    char fileBuffer[MAX_BUF];
    char fileSizeBuffer[16];

    char *authMessage = "AUTH OK\r\n";
    char *pathMessage = "PATH OK\r\n";
    char *sizeMessage = "SIZE OK\r\n";
    char *recvMessage = "RECV OK\r\n";
    char *errorMessage = "ERR\r\n";

    const char *dirs[5] = {"/", "Sales", "Promotions", "Offers", "Marketing"};

    FILE *fp;

    // Receive data from the client
    while ((READSIZE = recv(socket, message, MAX_BUF, 0)) > 0) {
        message[READSIZE] = 0;

        // Get files
        if (authenticated) {
            char *dashPtr = strchr(message,  '/');
            if (dashPtr != NULL) {
                // Get directory name from message
                if (strcmp(dashPtr, message) == 0) {
                    dirname[0] = '/';
                    dirname[1] = 0;
                }
                else {
                    dirname[0] = 0;
                    strcpy(dirname, message);
                    dirname[strcspn(dirname, "/")] = 0;
                }

                // Check if the directory received is correct
                int dirSelected = -1;
                for (int i = 0 ; i < 5 ; i ++) {
                    if (strcmp(dirs[i], dirname) == 0) {
                        dirSelected = i;
                        break;
                    }
                }

                if (dirSelected != -1) {
                    // Get local path of the file
                    if (strcmp(dashPtr, message) == 0) {
                        sprintf(path, "%s%s", INTRANET, message);
                    }
                    else {
                        sprintf(path, "%s/%s", INTRANET, message);
                    }

                    // Block access when writting to disk
                    pthread_mutex_lock(&lock_x);

                    // Write file to disk
                    fp = fopen(path, "w");
                    if (fp == NULL) {
                        printf("\nCannot create file\n");
                        send(socket, errorMessage, strlen(errorMessage), 0);
                    }
                    else {
                        bzero(fileBuffer, MAX_BUF);

                        // Inform the client if the directory name is correct
                        send(socket, pathMessage, strlen(pathMessage), 0);

                        // Get file size
                        while ((blockSize = recv(socket, fileSizeBuffer, 15, 0)) > 0) {
                            fileSizeBuffer[blockSize] = 0;

                            if (strstr(fileSizeBuffer, "\r\n")) {
                                break;
                            }
                        }
                        fileSize = atoi(fileSizeBuffer);

                        // Get file data
                        if (fileSize != 0) {
                            send(socket, sizeMessage, strlen(sizeMessage), 0);

                            // Get file data
                            int bytesRecv = 0;
                            while((blockSize = recv(socket, fileBuffer, MAX_BUF, 0)) > 0) {
                                int write_sz = fwrite(fileBuffer, sizeof(char), blockSize, fp);

                                // The no bytes written to file does not match the bytes received
                                if (write_sz != blockSize) {
                                    send(socket, errorMessage, strlen(sizeMessage), 0);
                                    break;
                                }

                                bzero(fileBuffer, MAX_BUF);

                                // Add the block size and check if the whole file was sent
                                bytesRecv += blockSize;
                                if (bytesRecv >= fileSize) {
                                    break;
                                }
                            }

                            // Inform the client the data has been received
                            send(socket, recvMessage, strlen(recvMessage), 0);
                            fflush(stdout);
                        }
                        else {
                            send(socket, errorMessage, strlen(errorMessage), 0);
                        }

                        fflush(fp);
                        fclose(fp);
                    }

                    // TODO: Log it
                    printf("\nLog\n");

                    // Unblock mutex
                    pthread_mutex_unlock(&lock_x);

                    fflush(stdout);
                }
                else {
                    send(socket, errorMessage, strlen(errorMessage), 0);
                }
            }
        }

        // Authenticate user
        if (!authenticated) {
            // Block access before accesing the htpasswd file
            pthread_mutex_lock(&lock_x);

            fp = fopen(HTPASSWD_PATH, "r");

            while(fgets(line, MAX_BUF, fp)) {
                // Remove newline from each line
                line[strcspn(line, "\n")] = 0;

                if (strcmp(line, message) == 0) {
                    authenticated = 1;
                    break;
                }
            }

            fclose(fp);

            // Unblock mutex
            pthread_mutex_unlock(&lock_x);

            if (authenticated) {
                send(socket, authMessage, strlen(authMessage), 0);
            }
            else {
                send(socket, errorMessage, strlen(errorMessage), 0);
            }
        }

        bzero(message, MAX_BUF);
    }

    if (READSIZE == 0) {
        printf("Client has disconected\n");
    }
    else if (READSIZE == -1) {
        perror("Read error");
    }

    pthread_exit(NULL);
}
