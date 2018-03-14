#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#include "macros.h"
#include "timelib.h"

int setupAudit() {
    // auditd

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
