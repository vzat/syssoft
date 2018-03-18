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
    fp = popen(cmd, "r");
    status = pclose(fp);

    // Send status to message queue
    mq = mq_open(QUEUE_NAME, O_WRONLY);

    if (status == 0) {
        sprintf(buffer, "<success> transfer: The website has been updated");
    }
    else {
        sprintf(buffer, "<error> transfer: The website cannot be updated. Returned value %d", status);
    }

    mq_send(mq, buffer, strlen(buffer), 0);
    mq_close(mq);

    // Exit with appropriate exit code
    if (status == 0) {
        exit(EXIT_SUCCESS);
    }
    else {
        exit(EXIT_FAILURE);
    }
}
