#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <mqueue.h>

#include "macros.h"

void backup() {
    mqd_t mq;
    char buffer[MAX_BUF];
    char cmd[MAX_CMD];

    FILE *fp;
    int status;

    sprintf(cmd, "rsync -avP %s %s", LIVE, BACKUP);

    // Run command and get status
    fp = fopen(cmd, "r");
    status = pclose(fp);

    // Send status to message queue
    mq = mq_open(QUEUE_NAME, O_WRONLY);

    if (status == 0) {
        sprintf(buffer, "success: backup: The website has been backed up");
    }
    else {
        sprintf(buffer, "error: backup: The website cannot be backed up");
    }

    mq_send(mq, buffer, strlen(buffer), 0);

    mq_close(mq);


    // printf("Backing up Live\n\n");
    //
    // // Trailing slash for the directories is necessary to copy just the contents!!!
    // execlp("rsync", "rsync", "-avP", LIVE, BACKUP, NULL);
    //
    // perror("Cannot backup files");
    // exit(1);
}
