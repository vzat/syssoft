#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "macros.h"

void *connection(void *arg) {
    int socket = * (int*) arg;

    int READSIZE;
    char message[MAX_BUF];

    while ((READSIZE = recv(socket, message, 2000, 0)) > 0) {
        printf("%s", message);
    }

    if (READSIZE == 0) {
        printf("Client has disconected\n");
    }
    else if (READSIZE == -1) {
        perror("Read error");
    }

    pthread_exit(NULL);
}
