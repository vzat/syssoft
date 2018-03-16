#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <mqueue.h>
#include <syslog.h>

#include "macros.h"
#include "timelib.h"

int setupAudit() {
    char cmd[MAX_CMD];

    FILE *fp;
    int status;

    // Remove Watch if it exists
    sprintf(cmd, "auditctl -W %s", INTRANET);

    // Run command and get status
    fp = popen(cmd, "r");
    status = pclose(fp);

    // Add Watch
    sprintf(cmd, "auditctl -w %s -p rwxa", INTRANET);

    // Run command and get status
    fp = popen(cmd, "r");
    status = pclose(fp);

    // If an error occurs return -1
    if (status != 0) {
        return -1;
    }

    return status;
}

void logChanges() {
    // ausearch from last log time (size of MAX_DATE)

    // mqd_t mq;
    size_t max_date = sizeof(char) * MAX_DATE;

    // char buffer[MAX_BUF];
    char cmd[MAX_CMD];

    FILE *fp;
    int status;

    // Remove trailing slash if it exists
    char * tempIntranet;
    if (strlen(strrchr(INTRANET, '/')) == 1) {
        tempIntranet = malloc(sizeof(char) * strlen(INTRANET));
        strncpy(tempIntranet, INTRANET, strlen(INTRANET) - 1);
    }
    else {
        tempIntranet = malloc(sizeof(char) * strlen(INTRANET) + 1);
        strncpy(tempIntranet, INTRANET, strlen(INTRANET));
    }

    // Get filename safe time
    char * timeFileNameSafe = malloc(max_date);
    getCurrentTimeFileNameSafe(timeFileNameSafe, max_date);

    // Get last log time
    char * lastLogTime = malloc(max_date);
    getCurrentTime(lastLogTime, max_date);
    syslog(LOG_INFO, "%s", lastLogTime);

    // Get all records from the last check
    sprintf(cmd, "ausearch -f %s -ts %s -i -l > %s%s.txt", tempIntranet, lastLogTime, AUDIT_PATH, timeFileNameSafe);
    // sprintf(cmd, "ausearch -f %s -ts %s -i -l", tempIntranet, lastLogTime);

    free(timeFileNameSafe);
    free(lastLogTime);

    // syslog(LOG_INFO, "Before %s", lastLogTime);

    fp = popen(cmd, "r");
    status = pclose(fp);

    syslog(LOG_INFO, "audit: %d", status);

    // while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    //     syslog(LOG_INFO, "LOG: %s", buffer);
    // }

    // Update the lastLogTime
    // getCurrentTime(lastLogTime, max_date);

    // syslog(LOG_INFO, "After %s", lastLogTime);
}
