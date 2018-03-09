#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <mqueue.h>
#include <syslog.h>

#include "macros.h"
#include "permissions.h"
#include "backup.h"
#include "transfer.h"

int main (int argc, char **argv) {

    // TODO: Create orphan process to deamonize this process

    mqd_t mq;

    struct mq_attr queue_attributes;
    char buffer[MAX_BUF + 1];

    int terminate = 0;

    int error = 0;

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
        printf("%s\n\n", "Loop");

        // Read message
        ssize_t bytes_read;
        bytes_read = mq_receive(mq, buffer, MAX_BUF, NULL);
        buffer[bytes_read] = '\0';

        printf("Buffer: %s\n\n", buffer);

        if (strstr(buffer, "error")) {
            error = 1;
        }
        else {
            error = 0;
        }

        if (!strncmp(buffer, "backup", strlen("backup"))) {
            // Block INTRANET access
            switch (fork()) {
                case 0:
                    changeMode(INTRANET, "000");
                    break;
                case -1:
                    syslog(LOG_ERR, "%s", "Cannot init fork");
                    break;
            }
        }

        if (!error && strstr(buffer, "permissions") && strstr(buffer, INTRANET) && strstr(buffer, "000")) {
            // Block BACKUP access
            switch (fork()) {
                case 0:
                    changeMode(BACKUP, "000");
                    break;
                case -1:
                    syslog(LOG_ERR, "%s", "Cannot init fork");
                    break;
            }
        }

        if (!error && strstr(buffer, "permissions") && strstr(buffer, BACKUP) && strstr(buffer, "777")) {
            // Backup
            switch (fork()) {
                case 0:
                    backup();
                    break;
                case -1:
                    syslog(LOG_ERR, "%s", "Cannot init fork");
                    break;
            }
        }

        if (!error && strstr(buffer, "backup")) {
            // Transfer
            switch (fork()) {
                case 0:
                    transfer();
                    break;
                case -1:
                    syslog(LOG_ERR, "%s", "Cannot init fork");
                    break;
            }
        }

        if (!error && strstr(buffer, "transfer")) {
            // Unblock INTRANET access
            switch (fork()) {
                case 0:
                    changeMode(INTRANET, "777");
                    break;
                case -1:
                    syslog(LOG_ERR, "%s", "Cannot init fork");
                    break;
            }
        }

        if (!error && strstr(buffer, "permissions") && strstr(buffer, "777")) {
              // Unblock BACKUP access
              switch (fork()) {
                  case 0:
                      changeMode(BACKUP, "777");
                      break;
                  case -1:
                      syslog(LOG_ERR, "%s", "Cannot init fork");
                      break;
              }
        }

        // Log Status
        if (strstr(buffer, "error")) {
            syslog(LOG_ERR, "%s", buffer);
        }
        else if (strstr(buffer, "success")) {
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
