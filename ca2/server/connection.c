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

    char message[MAX_BUF];
    char line[MAX_BUF];
    char dirname[MAX_BUF];
    char path[MAX_BUF];
    char fileBuffer[MAX_BUF];

    char *authMessage = "auth\r\n";
    char *errorAuth = "err\r\n";
    char *pathMessage = "path\r\n";
    char *errorPath = "err\r\n";

    const char *dirs[5] = {"/", "Sales", "Promotions", "Offers", "Marketing"};

    FILE *fp;

    // Receive data from the client
    while ((READSIZE = recv(socket, message, MAX_BUF, 0)) > 0) {
        message[READSIZE] = 0;

        // Get files
        if (authenticated) {
              printf("\nmessage: %s\n", message);

              if (strchr(message, '/') == NULL) {
                  printf("NULL");
              }
              else {
                  printf("NOT NULL");
              }

              // Get directory name from message
              if (strcmp(strchr(message, '/'), message) == 0) {
                  printf("\n???\n");
                  dirname[0] = '/';
                  dirname[1] = 0;
              }
              else {
                  printf("\nBefore strcpy\n");
                  dirname[0] = 0;
                  strcpy(dirname, message);
                  printf("\nbefore removing slash: %s\n", dirname);
                  dirname[strcspn(dirname, "/")] = 0;
              }

              printf("\ndirname: %s\n", dirname);

              // Check if the directory received is correct
              int dirSelected = -1;
              for (int i = 0 ; i < 5 ; i ++) {
                  if (strcmp(dirs[i], dirname) == 0) {
                      dirSelected = i;
                      break;
                  }
              }

              // Inform the client if the directory name is correct
              if (dirSelected != -1) {
                  send(socket, pathMessage, strlen(pathMessage), 0);

                  // Block access when writting to disk
                  pthread_mutex_lock(&lock_x);

                  // Get local path of the file
                  if (strcmp(strchr(message, '/'), message) == 0) {
                      sprintf(path, "%s%s", INTRANET, message);
                  }
                  else {
                      sprintf(path, "%s/%s", INTRANET, message);
                  }

                  // Write file to disk
                  printf("\npath: %s\n", path);
                  if ((fp = fopen(path, "w")) == NULL) {
                      printf("\nCannot create file\n");
                  }
                  else {
                      bzero(fileBuffer, MAX_BUF);

                      while((blockSize = recv(socket, fileBuffer, MAX_BUF, 0)) > 0) {
                          fwrite(fileBuffer, sizeof(char), blockSize, fp);
                          bzero(fileBuffer, MAX_BUF);
                      }
                  }

                  fclose(fp);

                  // TODO: Log it

                  // Unblock mutex
                  pthread_mutex_unlock(&lock_x);
              }
              else {
                  send(socket, errorPath, strlen(errorPath), 0);
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
                send(socket, errorAuth, strlen(errorAuth), 0);
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
