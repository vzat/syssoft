#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>

#define MAX_BUF 1024

int main(void) {
    mqd_t mq;

    char buffer[MAX_BUF];

    // Open message queue
    mq = mq_open("/daemonQ", O_WRONLY);

    printf("Send message to server (enter 'exit' to terminate):\n");

    do {
        printf(">> ");
        fflush(stdout);

        memset(buffer, 0, MAX_BUF);
        fgets(buffer, MAX_BUF, stdin);
        mq_send(mq, buffer, MAX_BUF, 0);
    } while (strncmp(buffer, "exit", strlen("exit")));

    mq_close(mq);

    return 0;
}
