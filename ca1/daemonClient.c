#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>

#include "macros.h"

int main (int argc, char **argv) {
    mqd_t mq;

    char buffer[MAX_BUF];

    // Open message queue
    mq = mq_open(QUEUE_NAME, O_WRONLY);

    printf("Send message to server. The following commands are available:\n");
    printf("Enter 'quit' to terminate client\n");
    printf("Enter 'exit' to terminate server\n");
    printf("Enter 'backup' to backup and transfer the website directory\n\n");

    do {
        printf(">> ");
        fflush(stdout);

        memset(buffer, 0, MAX_BUF);
        fgets(buffer, MAX_BUF, stdin);
        int returnValue = mq_send(mq, buffer, MAX_BUF, 0);
        printf("Status: %d\n", returnValue);
    } while (strncmp(buffer, "quit", strlen("quit")));

    mq_close(mq);

    return 0;
}
