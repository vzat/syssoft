#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <mqueue.h>
#include <syslog.h>
#include <errno.h>

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

    // Create log
    openlog("serverdaemon", LOG_PID | LOG_CONS, LOG_USER);

    // Message Queue Attributes
    queue_attributes.mq_flags = 0;
    queue_attributes.mq_maxmsg = 10;
    queue_attributes.mq_msgsize = MAX_BUF;
    queue_attributes.mq_curmsgs = 0;

    // Create Queue
    syslog(LOG_INFO, "Creating Message Queue %s", QUEUE_NAME);
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &queue_attributes);

    if (mq == -1) {
        syslog(LOG_INFO, "Cannot create queue: %s", strerror(errno));
    }

    do {
        syslog(LOG_INFO, "Waiting for messages");

        // Read message
        ssize_t bytes_read;
        memset(buffer, 0, MAX_BUF);
        bytes_read = mq_receive(mq, buffer, MAX_BUF, NULL);
        buffer[bytes_read] = '\0';

        syslog(LOG_INFO, "New message in the queue");
        printf("New message in the queue\n\n");

        if (strstr(buffer, "error")) {
            error = 1;
        }
        else {
            error = 0;
        }

        if (!strncmp(buffer, "backup", strlen("backup"))) {
            // Block INTRANET access
            syslog(LOG_INFO, "Changing INTRANET permissions to 000");
            switch (fork()) {
                case 0:
                    changeMode(INTRANET, "000");
                    break;
                case -1:
                    syslog(LOG_ERR, "%s: %s", "Cannot init fork", strerror(errno));
                    break;
            }
        }

        if (!error && strstr(buffer, "permissions") && strstr(buffer, INTRANET) && strstr(buffer, "000")) {
            // Block BACKUP access
            syslog(LOG_INFO, "Changing BACKUP permissions to 000");
            switch (fork()) {
                case 0:
                    changeMode(BACKUP, "000");
                    break;
                case -1:
                    syslog(LOG_ERR, "%s: %s", "Cannot init fork", strerror(errno));
                    break;
            }
        }

        if (!error && strstr(buffer, "permissions") && strstr(buffer, BACKUP) && strstr(buffer, "777")) {
            // Backup
            syslog(LOG_INFO, "Backup data");
            switch (fork()) {
                case 0:
                    backup();
                    break;
                case -1:
                    syslog(LOG_ERR, "%s: %s", "Cannot init fork", strerror(errno));
                    break;
            }
        }

        if (!error && strstr(buffer, "backup")) {
            // Transfer
            syslog(LOG_INFO, "Transfer the new files from intranet to live");
            switch (fork()) {
                case 0:
                    transfer();
                    break;
                case -1:
                    syslog(LOG_ERR, "%s: %s", "Cannot init fork", strerror(errno));
                    break;
            }
        }

        if (!error && strstr(buffer, "transfer")) {
            // Unblock INTRANET access
            syslog(LOG_INFO, "Change INTRANET permissions to 777");
            switch (fork()) {
                case 0:
                    changeMode(INTRANET, "777");
                    break;
                case -1:
                    syslog(LOG_ERR, "%s: %s", "Cannot init fork", strerror(errno));
                    break;
            }
        }

        if (!error && strstr(buffer, "permissions") && strstr(buffer, "777")) {
              // Unblock BACKUP access
              syslog(LOG_INFO, "Changing BACKUP permissions to 777");
              switch (fork()) {
                  case 0:
                      changeMode(BACKUP, "777");
                      break;
                  case -1:
                      syslog(LOG_ERR, "%s: %s", "Cannot init fork", strerror(errno));
                      break;
              }
        }

        // Log Status
        if (strstr(buffer, "error")) {
            syslog(LOG_ERR, "%s", buffer);
            printf("%s", buffer);
        }
        else if (strstr(buffer, "success")) {
            syslog(LOG_INFO, "%s", buffer);
            printf("%s", buffer);
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
