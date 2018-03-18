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
#include "logger.h"

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
    char * logOutput = "<info> daemon: Backup and Transfer has been completed (BACKUP-COMPLETED)";
    mq = mq_open(QUEUE_NAME, O_WRONLY);
    mq_send(mq, logOutput, strlen(logOutput), 0);
    mq_close(mq);
}

int main (int argc, char **argv) {

    // Deamonize process
    int pid = fork();

    if (pid > 0) {
        // Parent process
        sleep(5);
        exit(EXIT_SUCCESS);
    }

    if (pid == -1) {
        exit(EXIT_FAILURE);
    }

    // Elavate orphan to session leader
    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }

    // Set file mode creation to 0
    umask(0);

    // Change working dir to /
    if (chdir("/") < 0) {
        exit(EXIT_FAILURE);
    }

    // Close all file descriptors
    for (int x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
        close(x);
    }

    // Daemon
    mqd_t mq;

    struct mq_attr queue_attributes;
    char buffer[MAX_BUF + 1];

    int terminate = 0;
    int backupBlocked = 0;

    char * scheduledMessage = "backup";

    int hour = 2, min = 0, sec = 0;

    int fd[2];

    // If scheduled time is passed as command line arguments
    // replace the default values
    if (argc == 4) {
        hour = atoi(argv[1]);
        min = atoi(argv[2]);
        sec = atoi(argv[3]);
    }

    // Create pipe for logger
    pipe(fd);

    // Tail syslog
    switch (fork()) {
        case 0:
            pipeSyslogToFile(fd);
            break;
        case -1:
            syslog(LOG_ERR, "%s: %s", "<error> daemon: Cannot init fork", strerror(errno));
            break;
    }

    // Take no input
    close(fd[0]);

    // Set the current time as the last log message
    setCurrentTime();

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

    // Create audit and logs directories
    // Use 555 so anyone can read the files but only
    // the superuser can remove or change them
    mkdir(AUDIT_PATH, 0555);
    mkdir(LOG_PATH, 0555);

    // Setup audit daemon
    if (setupAudit() == -1) {
        syslog(LOG_ERR, "<error> daemon: Cannot setup audit");
        exit(EXIT_FAILURE);
    }
    else {
        syslog(LOG_INFO, "<info> daemon: The audit has been setup");
    }

    // Schedule backup
    switch (fork()) {
        case 0:
            waitForTime(hour, min, sec, scheduledMessage, strlen(scheduledMessage));
            break;
        case -1:
            syslog(LOG_ERR, "%s: %s", "<error> daemon: Cannot init fork", strerror(errno));
            break;
    }

    do {
        // Read message
        ssize_t bytes_read;
        memset(buffer, 0, MAX_BUF);
        bytes_read = mq_receive(mq, buffer, MAX_BUF, NULL);
        buffer[bytes_read] = '\0';

        // If an error or BACKUP-COMPLETED message is received from the message queue
        // then unblock backing up
        if (strstr(buffer, "error") || strstr(buffer, "BACKUP-COMPLETED")) {
            backupBlocked = 0;
        }

        // Triggered after the backup is completed
        if (strstr(buffer, "BACKUP-COMPLETED")) {
            // Set the new last audit log time as the current time
            setCurrentTime();
        }

        // Triggered when the schedule time is reached
        if (strstr(buffer, "TIME-REACHED")) {
            // Schedule task for the next day
            switch (fork()) {
                case 0:
                    waitForTime(hour, min, sec, scheduledMessage, strlen(scheduledMessage));
                    break;
                case -1:
                    syslog(LOG_ERR, "%s: %s", "<error> daemon: Cannot init fork", strerror(errno));
                    break;
            }
        }

        // If a backup message is received from the message queue
        // backup and transfer the data
        if (!backupBlocked && !strncmp(buffer, "backup", strlen("backup"))) {
            backupBlocked = 1;
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

    close(fd[1]);
    mq_close(mq);
    mq_unlink(QUEUE_NAME);
    closelog();

    return 0;
}
