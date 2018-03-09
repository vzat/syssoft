#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mqueue.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "macros.h"

void blockAccess() {
    printf("Blocking access to files\n\n");

    execlp("chmod", "chmod", "000", INTRANET, LIVE, BACKUP, NULL);

    perror("Cannot change permissions");
    exit(1);
}

void unblockAccess() {
    printf("Unblock access to files\n\n");

    execlp("chmod", "chmod", "777", INTRANET, BACKUP, NULL);

    perror("Cannot change permissions");
    exit(1);
}

void changeMode(char * dir, char * mode) {
    mqd_t mq;
    char buffer[MAX_BUF];
    char cmd[MAX_CMD];

    FILE *fp;
    int status;

    sprintf(cmd, "chmod %s %s", mode, dir);

    // Run command and get status
    fp = popen(cmd, "r");
    status = pclose(fp);

    // Send status to message queue
    mq = mq_open(QUEUE_NAME, O_WRONLY);

    if (status == 0) {
        sprintf(buffer, "success: permissions: Changed mode of directory %s to %s", dir, mode);
    }
    else {
        sprintf(buffer, "error: permissions: Cannot change mode of directory %s to %s", dir, mode);
    }

    mq_send(mq, buffer, strlen(buffer), 0);

    mq_close(mq);
}
