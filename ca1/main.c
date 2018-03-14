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

void backupAndTransferError() {
    // Send status to daemon
    mqd_t mq;
    mq = mq_open(QUEUE_NAME, O_WRONLY);
    mq_send(mq, "<error> daemon: Backup and Transfer cannot be completed", strlen("<error> daemon: Backup and Transfer cannot be completed"), 0);
    mq_close(mq);
}

void backupAndTransfer () {
    int status;
    pid_t pid;

    // Block INTRANET access
    syslog(LOG_INFO, "<info> daemon: Changing INTRANET permissions to 000");
    pid = fork();
    switch (pid) {
        case 0:
            changeMode(INTRANET, "000");
            break;
        case -1:
            syslog(LOG_ERR, "%s: %s", "<error> daemon: Cannot init fork", strerror(errno));
            break;
    }

    waitpid(pid, &status, 0);

    // Send error message to daemon
    if (status != 0) {
        backupAndTransferError();
        return;
    }

    // Block BACKUP access
    syslog(LOG_INFO, "<info> daemon: Changing BACKUP permissions to 000");
    pid = fork();
    switch (pid) {
        case 0:
            changeMode(BACKUP, "000");
            break;
        case -1:
            syslog(LOG_ERR, "%s: %s", "<error> daemon: Cannot init fork", strerror(errno));
            break;
    }

    waitpid(pid, &status, 0);

    // Send error message to daemon
    if (status != 0) {
        backupAndTransferError();
        return;
    }

    // Backup
    syslog(LOG_INFO, "<info> daemon: Backup data");
    pid = fork();
    switch (pid) {
        case 0:
            backup();
            break;
        case -1:
            syslog(LOG_ERR, "%s: %s", "<error> daemon: Cannot init fork", strerror(errno));
            break;
    }

    waitpid(pid, &status, 0);

    // Send error message to daemon
    if (status != 0) {
        backupAndTransferError();
        return;
    }

    // Transfer
    syslog(LOG_INFO, "<info> daemon: Transfer the new files from intranet to live");
    pid = fork();
    switch (pid) {
        case 0:
            transfer();
            break;
        case -1:
            syslog(LOG_ERR, "%s: %s", "<error> daemon: Cannot init fork", strerror(errno));
            break;
    }

    waitpid(pid, &status, 0);

    // Send error message to daemon
    if (status != 0) {
        backupAndTransferError();
        return;
    }

    // Unblock INTRANET access
    syslog(LOG_INFO, "<info> daemon: Changing INTRANET permissions to 777");
    pid = fork();
    switch (pid) {
        case 0:
            changeMode(INTRANET, "777");
            break;
        case -1:
            syslog(LOG_ERR, "%s: %s", "<error> daemon: Cannot init fork", strerror(errno));
            break;
    }

    waitpid(pid, &status, 0);

    // Send error message to daemon
    if (status != 0) {
        backupAndTransferError();
        return;
    }

    // Unblock BACKUP access
    syslog(LOG_INFO, "<info> daemon: Changing BACKUP permissions to 777");
    pid = fork();
    switch (pid) {
        case 0:
            changeMode(BACKUP, "777");
            break;
        case -1:
            syslog(LOG_ERR, "%s: %s", "<error> daemon: Cannot init fork", strerror(errno));
            break;
    }

    waitpid(pid, &status, 0);

    // Send error message to daemon
    if (status != 0) {
        backupAndTransferError();
        return;
    }

    // Send status to daemon
    mqd_t mq;
    mq = mq_open(QUEUE_NAME, O_WRONLY);
    mq_send(mq, "<done> daemon: Backup and Transfer has been completed", strlen("<done> daemon: Backup and Transfer has been completed"), 0);
    mq_close(mq);
}

int main (int argc, char **argv) {

    // TODO: Create orphan process to deamonize this process

    mqd_t mq;

    struct mq_attr queue_attributes;
    char buffer[MAX_BUF + 1];

    int terminate = 0;

    int error = 0;
    int backupBlocked = 0;

    // Create log
    openlog("serverdaemon", LOG_PID | LOG_CONS, LOG_USER);

    // Message Queue Attributes
    queue_attributes.mq_flags = 0;
    queue_attributes.mq_maxmsg = 10;
    queue_attributes.mq_msgsize = MAX_BUF;
    queue_attributes.mq_curmsgs = 0;

    // Create Queue
    syslog(LOG_INFO, "<info> daemon: Creating Message Queue %s", QUEUE_NAME);
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &queue_attributes);

    if (mq == -1) {
        syslog(LOG_INFO, "<error> daemon: Cannot create queue: %s", strerror(errno));
    }

    do {
        // Read message
        ssize_t bytes_read;
        memset(buffer, 0, MAX_BUF);
        bytes_read = mq_receive(mq, buffer, MAX_BUF, NULL);
        buffer[bytes_read] = '\0';

        if (strstr(buffer, "error") || strstr(buffer, "done")) {
            backupBlocked = 0;
        }

        if (!backupBlocked && !strncmp(buffer, "backup", strlen("backup"))) {
            switch (fork()) {
                case 0:
                    backupAndTransfer();
                    break;
                case -1:
                    syslog(LOG_ERR, "%s: %s", "<error> daemon: Cannot init fork", strerror(errno));
                    break;
            }
        }

        // Log Status
        if (strstr(buffer, "error")) {
            syslog(LOG_ERR, "%s", buffer);
        }
        else if (strchr(buffer, '<') && strchr(buffer, '>')) {
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
