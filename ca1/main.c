#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <mqueue.h>
#include <syslog.h>

#include "macros.h"
#include "timelib.h"

int main (int argc, char **argv) {

    // TODO: Create orphan process to deamonize this process

    mqd_t mq;

    struct mq_attr queue_attributes;
    char buffer[MAX_BUF + 1];

    int terminate = 0;

    // Message Queue Attributes
    queue_attributes.mq_flags = 0;
    queue_attributes.mq_maxmsg = 10;
    queue_attributes.mq_msgsize = MAX_BUF;
    queue_attributes.mq_curmsgs = 0;

    // Create log
    openlog("serverdaemon", LOG_PID | LOG_CONS, LOG_USER);

    // Create Queue
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &queue_attributes);

    do {
        printf("%s\n\n", "New Loop");

        // Read message
        ssize_t bytes_read;
        bytes_read = mq_receive(mq, buffer, MAX_BUF, NULL);
        buffer[bytes_read] = '\0';

        if (strstr(buffer, "backup")) {
            // Backup and Transfer
            printf("%s\n\n", "BACKUP");
        }

        // Log Status
        if (!strcmp(buffer, "error")) {
            syslog(LOG_ERR, "%s", buffer);
        }
        else {
            syslog(LOG_INFO, "%s", buffer);
        }

        // Terminate Daemon
        if (!strncmp(buffer, "exit", strlen("exit"))) {
            terminate = 1;
        }
    } while (!terminate);

    mq_close(mq);
    mq_unlink(QUEUE_NAME);
    closelog();

    return 0;
}
