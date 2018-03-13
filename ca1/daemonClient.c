#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>

#define MAX_BUF 1024

int main(int argc, char **argv) {
    mqd_t mq;

    char buffer[MAX_BUF];

    if (argc != 2) {
        printf("This program requires a queue name as parameter\n\n");
        exit(1);
    }

    // Open message queue
    mq = mq_open(argv[1], O_WRONLY);

    printf("Send message to server. The following commands are available:\n");
    printf("Enter 'quit' to terminate client\n");
    printf("Enter 'exit' to termiante server\n");
    printf("Enter 'backup' to backup and transfer the website directory\n\n");

    do {
        printf(">> ");
        fflush(stdout);

        memset(buffer, 0, MAX_BUF);
        fgets(buffer, MAX_BUF, stdin);
        mq_send(mq, buffer, MAX_BUF, 0);
    } while (strncmp(buffer, "quit", strlen("quit")));

    mq_close(mq);

    return 0;
}
