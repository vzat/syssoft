#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#include "macros.h"

int SID;

void sig_handler(int sigNum) {
    if (sigNum == SIGINT) {
        printf("\n\n=== Goodbye ===\n");
        close(SID);
        exit(EXIT_SUCCESS);
    }
}

int main (int argc, char ** argv) {
    int authenticated = 0;
    int recvBytes;
    int blockSize;
    int fileSize = 0;

    struct sockaddr_in server;

    char username[100];
    char password[100];
    char filename[100];
    char path[100];

    char clientMessage[MAX_BUF + 1];
    char serverMessage[MAX_BUF + 1];
    char fileBuffer[MAX_BUF];
    char fileSizeBuffer[16];

    FILE *fp;

    // Create Socket
    SID = socket(AF_INET, SOCK_STREAM, 0);
    if (SID == -1) {
        printf("Error creating socket\n");
    }
    else {
        printf("Socket created\n");
    }

    // Init Socket
    server.sin_port = htons(8081);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;

    if (connect(SID, (struct sockaddr *) &server, sizeof(server)) < 0) {
        printf("Connection failed\n");
        return 1;
    }

    printf("Successfully connected to the server\n");

    if (signal(SIGINT, sig_handler) == SIG_ERR) {
      printf("Cannot intercept signal\n");
    }

    while (1) {
        if (authenticated) {
            printf("\nFilename: ");
            scanf("%s", filename);

            printf("Path: ");
            scanf("%s", path);

            // Check if the file exists
            if ((fp = fopen(filename, "r")) == NULL) {
                printf("\n=== The file cannot be found ===\n");
            }
            else {
                // Remove forward slash from each line
                path[strcspn(path, "/")] = 0;

                bzero(clientMessage, MAX_BUF);

                // Get just the name of the file without the local path
                char *lastSlashPtr = strrchr(filename, '/');
                if (lastSlashPtr == NULL) {
                    sprintf(clientMessage, "%s/%s", path, filename);
                }
                else {
                    sprintf(clientMessage, "%s%s", path, lastSlashPtr);
                }

                if (send(SID, clientMessage, strlen(clientMessage), 0) == -1) {
                    printf("\n=== Server not reachable ===\n");
                }
                else {
                    // Receive reply from server
                    int pathAccepted = 0;
                    while ((recvBytes = recv(SID, serverMessage, MAX_BUF, 0)) > 0) {
                        serverMessage[recvBytes] = 0;

                        if (strstr(serverMessage, "PATH OK")) {
                            pathAccepted = 1;
                        }
                        else {
                            printf("\n=== Invalid Filename and/or Path ===\n");
                        }

                        if (strstr(serverMessage, "\r\n")) {
                            break;
                        }
                    }

                    // Send file
                    if (pathAccepted) {
                        printf("\n=== Sending file... ===\n");
                        bzero(fileBuffer, MAX_BUF);

                        // Get filesize
                        fseek(fp, 0, SEEK_END);
                        fileSize = ftell(fp);

                        // Send filesize
                        sprintf(fileSizeBuffer, "%d\r\n", fileSize);
                        send(SID, fileSizeBuffer, strlen(fileSizeBuffer), 0);

                        // Get server response
                        while ((recvBytes = recv(SID, serverMessage, MAX_BUF, 0)) > 0) {
                            serverMessage[recvBytes] = 0;

                            if (strstr(serverMessage, "\r\n")) {
                                break;
                            }
                        }

                        // Check if the server received the filesize
                        if (strstr(serverMessage, "SIZE OK") != NULL) {
                            // Go to the begining of the file
                            fseek(fp, 0, SEEK_SET);

                            while ((blockSize = fread(fileBuffer, sizeof(char), MAX_BUF, fp)) > 0) {
                                if (send(SID, fileBuffer, blockSize, 0) < 0) {
                                    printf("\n=== Server not reachable ===\n");
                                    return 1;
                                }
                                bzero(fileBuffer, MAX_BUF);
                            }

                            fclose(fp);

                            // Check if the file was received
                            while ((recvBytes = recv(SID, serverMessage, MAX_BUF, 0)) > 0) {
                                serverMessage[recvBytes] = 0;

                                if (strstr(serverMessage, "RECV OK")) {
                                    printf("\n=== The file was received ===\n");
                                }
                                else {
                                    printf("\n=== The file was not received ===\n");
                                }

                                if (strstr(serverMessage, "\r\n")) {
                                    break;
                                }
                            }
                        }
                        else {
                            printf("\n=== Invalid filesize ===\n");
                        }
                    }
                }
            }
        }
        else {
            printf("\nUsername: ");
            scanf("%s", username);

            printf("Password: ");
            scanf("%s", password);

            sprintf(clientMessage, "%s:%s", username, password);

            // Send credentials
            if (send(SID, clientMessage, strlen(clientMessage), 0) < 0) {
                printf("Send failed");
                return 1;
            }

            // Receive reply from server
            while ((recvBytes = recv(SID, serverMessage, MAX_BUF, 0)) > 0) {
                serverMessage[recvBytes] = 0;

                if (strstr(serverMessage, "AUTH OK")) {
                    printf("\n=== Authenticated ===\n");
                    authenticated = 1;
                }
                else {
                    printf("\n=== Invalid Username and/or Password ===\n");
                }

                if (strstr(serverMessage, "\r\n")) {
                    break;
                }
            }
        }
    }
}
