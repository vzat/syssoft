#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <mqueue.h>

#include "macros.h"

void transfer() {
    mqd_t mq;
    char buffer[MAX_BUF];
    char cmd[MAX_CMD];

    FILE *fp;
    int status;

    sprintf(cmd, "rsync -avP %s %s", INTRANET, LIVE);

    // Run command and get status
    fp = fopen(cmd, "r");
    status = pclose(fp);

    // Send status to message queue
    mq = mq_open(QUEUE_NAME, O_WRONLY);

    if (status == 0) {
        sprintf(buffer, "success: transfer: The website has been updated");
    }
    else {
        sprintf(buffer, "error: transfer: The website cannot be updated");
    }

    mq_send(mq, buffer, strlen(buffer), 0);

    mq_close(mq);


    // printf("Transfering changes from Intranet to Live\n\n");
    //
    // execlp("rsync", "rsync", "-avP", INTRANET, LIVE, NULL);
    //
    // perror("Cannot transfer changes to live");
    // exit(1);
}
