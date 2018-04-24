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
    char message[MAX_BUF];
    char line[MAX_BUF];

    char *authMessage = "Authenticated";
    char *errorAuth = "Invalid credentials";

    // Receive data from the client
    while ((READSIZE = recv(socket, message, MAX_BUF, 0)) > 0) {
        // Get files
        if (authenticated) {
            printf("Authenticated");
        }

        // Authenticate user
        if (!authenticated) {
            // Block access before accesing the htpasswd file
            pthread_mutex_lock(&lock_x);

            FILE *fp;
            fp = fopen(HTPASSWD_PATH, "r");

            while(fgets(line, MAX_BUF, fp)) {
                // Remove newline from each line
                line[strcspn(line, "\n")] = 0;

                if (strcmp(line, message) == 0) {
                    authenticated = 1;
                    break;
                }
            }

            // Unblock mutex
            pthread_mutex_unlock(&lock_x);

            if (authenticated) {
                send(socket, authMessage, sizeof(authMessage), 0);
            }
            else {
                send(socket, errorAuth, sizeof(errorAuth), 0);
            }
        }
    }

    if (READSIZE == 0) {
        printf("Client has disconected\n");
    }
    else if (READSIZE == -1) {
        perror("Read error");
    }

    pthread_exit(NULL);
}
