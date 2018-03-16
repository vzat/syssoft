#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <mqueue.h>
#include <syslog.h>
#include <errno.h>

#include "macros.h"
#include "permissions.h"
#include "backup.h"
#include "transfer.h"
#include "audit.h"
#include "timelib.h"

extern char * commonTime;

void backupAndTransferError () {
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

    // Output Changes to Audit
    syslog(LOG_INFO, "<info> daemon: Creating log with the latest changes");
    pid = fork();
    switch (pid) {
        case 0:
            logChanges();
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
    char * logOutput = "<done> daemon: Backup and Transfer has been completed";
    mq = mq_open(QUEUE_NAME, O_WRONLY);
    mq_send(mq, logOutput, strlen(logOutput), 0);
    mq_close(mq);
}

int main (int argc, char **argv) {

    // TODO: Create orphan process to deamonize this process

    mqd_t mq;

    struct mq_attr queue_attributes;
    char buffer[MAX_BUF + 1];

    int terminate = 0;
    int backupBlocked = 0;

    // Get the current time
    // size_t max_date = sizeof(char) * MAX_DATE;
    // char * lastLogTime = malloc(max_date);
    setCurrentTime();
    // getCurrentTime(lastLogTime, max_date);

    // Create log
    openlog(IDENT, LOG_PID | LOG_CONS, LOG_USER);

    // Message Queue Attributes
    queue_attributes.mq_flags = 0;
    queue_attributes.mq_maxmsg = 10;
    queue_attributes.mq_msgsize = MAX_BUF;
    queue_attributes.mq_curmsgs = 0;

    // Create Queue
    syslog(LOG_INFO, "<info> daemon: Creating Message Queue %s", QUEUE_NAME);
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &queue_attributes);

    if (mq == -1) {
        syslog(LOG_ERR, "<error> daemon: Cannot create queue: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else {
        syslog(LOG_INFO, "<info> daemon: Successfully created message queue");
    }

    // Setup audit daemon
    if (setupAudit() == -1) {
        syslog(LOG_ERR, "<error> daemon: Cannot initialise daemon");
        exit(EXIT_FAILURE);
    }
    else {
        syslog(LOG_INFO, "<info> daemon: The daemon has been initialised");
    }

    // Create audit and logs directories
    mkdir(AUDIT_PATH, 0777);
    mkdir(LOGS_PATH, 0777);


    do {
        // Read message
        ssize_t bytes_read;
        memset(buffer, 0, MAX_BUF);
        bytes_read = mq_receive(mq, buffer, MAX_BUF, NULL);
        buffer[bytes_read] = '\0';

        if (strstr(buffer, "error") || strstr(buffer, "done")) {
            backupBlocked = 0;
        }

        if (strstr(buffer, "done")) {
            setCurrentTime();
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

    // free(lastLogTime);
    mq_close(mq);
    mq_unlink(QUEUE_NAME);
    closelog();

    return 0;
}
