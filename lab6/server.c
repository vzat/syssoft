#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>

#define MAX_BUF 1024

int main(void) {
    mqd_t mq;

    struct mq_attr queue_attributes;
    char buffer[MAX_BUF + 1];
    int terminate = 0;

    // Queue Attributes
    queue_attributes.mq_flags = 0;
    queue_attributes.mq_maxmsg = 10;
    queue_attributes.mq_msgsize = MAX_BUF;
    queue_attributes.mq_curmsgs = 0;

    // Create Queue
    mq = mq_open("/myQueue", O_CREAT | O_RDONLY, 0644, &queue_attributes);

    do {
        ssize_t bytes_read;

        // Receive message
        bytes_read = mq_receive(mq, buffer, MAX_BUF, NULL);

        buffer[bytes_read] = '\0';

        if (! strncmp(buffer, "exit", strlen("exit"))) {
            terminate = 1;
        }
        else {
            printf("Received: %s\n\n", buffer);
        }
    } while (!terminate);

    mq_close(mq);
    mq_unlink("/myQueue");

    return 0;
}
