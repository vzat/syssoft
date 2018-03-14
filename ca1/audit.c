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

void logChanges(char * lastLogTime) {
    // ausearch from last log time (size of MAX_DATE)

    mqd_t mq;
    char buffer[MAX_BUF];
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

    // Get all records from the last check
    // sprintf(cmd, "ausearch -f %s -ts %s -i -l > %s%s.txt", tempIntranet, lastLogTime, BASE_PATH, lastLogTime);
    sprintf(cmd, "ausearch -f %s -ts %s -i -l", tempIntranet, lastLogTime);

    fp = popen(cmd, "r");

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        syslog(LOG_INFO, "LOG: %s", buffer);
    }

    // Update the lastLogTime
    size_t max_date = sizeof(char) * MAX_DATE;
    getCurrentTime(lastLogTime, max_date);
}


// int main(void) {
//
//     // Use auditctl to watch a dir
//     // Ocassionally or each time the file is backed up use ausearch to get the modified file
//     //    Filter the data to get only what is needed and put it in a file
//
//     pid_t pid;
//     int status;
//
//     char * now = malloc(sizeof(char) * MAX_DATE);
//     now = getCurrentDate(now);
//
//     // Get last checked record
//     printf("Last Time %s\n\n", now);
//
//     free(now);
//
//     return 0;
// }
