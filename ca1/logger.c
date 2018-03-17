#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>

#include "macros.h"
#include "timelib.h"

void pipeSyslogToFile(int fd[2]) {    
    mqd_t mq;
    char buffer[MAX_BUF];
    char cmd[MAX_CMD];

    FILE * fp;
    // FILE * fpPipe;
    int status;

    // Send no output
    close(fd[1]);

    // fpPipe = fdopen(fd[0], "r");

    // Get filename safe time
    size_t max_date = sizeof(char) * MAX_DATE;
    char * timeFileNameSafe = malloc(max_date);
    getCurrentTimeFileNameSafe(timeFileNameSafe, max_date);

    sprintf(cmd, "tail -f /var/log/syslog -n 0 | grep --line-buffered %s > %s%s.log", IDENT, LOG_PATH, timeFileNameSafe);

    free(timeFileNameSafe);

    // Run tail
    fp = popen(cmd, "r");

    // Wait data from pipe
    read(fd[0], buffer, MAX_BUF);
    // while (fgets(buffer, MAX_BUF, fpPipe));
    close(fd[0]);

    status = pclose(fp);

    // Send status to message queue
    mq = mq_open(QUEUE_NAME, O_WRONLY);

    memset(buffer, 0, MAX_BUF);
    if (status == 0) {
        sprintf(buffer, "<success> logger: The log has been successfully closed");
    }
    else {
        sprintf(buffer, "<error> logger: Cannot close log. Returned value %d", status);
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
