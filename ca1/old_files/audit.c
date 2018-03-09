#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#define INTRANET "/var/www/html/intranet/"
#define LIVE "/var/www/html/live/"
#define BACKUP "/var/www/html/backup/"
#define AUDIT_KEY "intranet.audit"
#define MAX_BUF 1024
#define MAX_CMD 128
#define MAX_DATE 1024

// char* getTimeAfterLastRecord() {
//     printf("Getting the time after the last record\n\n");
//
//     FILE *fp;
//     int status;
//     char buffer[MAX_BUF];
//     char cmd[MAX_CMD];
//
//     sprintf(cmd, "ausearch -k %s -i", AUDIT_KEY);
//     fp = popen(cmd, "r");
//
//     while (fgets(buffer, sizeof(buffer), fp) != NULL) {
//         char * timeStart = strstr(buffer, "audit(");
//         char * timeEnd = strchr(timeStart, '.');
//
//         if (timeStart != NULL || timeEnd != NULL) {
//               // Time is after audit(
//               timeStart += 6;
//               int timeLength = timeEnd - timeStart;
//               char * lastTime = (char *) malloc(timeLength + 1);
//               strncpy(lastTime, buffer, timeLength);
//               lastTime[timeLength] = '\0';
//
//               return lastTime;
//         }
//     }
//
//     perror("No Records Found");
//     return NULL;
// }

char * getCurrentDate(char * nowString) {
    time_t now;
    struct tm * nowInfo;

    time(&now);
    nowInfo = localtime(&now);

    strftime(nowString, MAX_DATE, "%d/%m/%Y %H:%M:%S", nowInfo);

    return nowString;
}



int main(void) {

    // Use auditctl to watch a dir
    // Ocassionally or each time the file is backed up use ausearch to get the modified file
    //    Filter the data to get only what is needed and put it in a file

    pid_t pid;
    int status;

    char * now = malloc(sizeof(char) * MAX_DATE);
    now = getCurrentDate(now);

    // Get last checked record
    printf("Last Time %s\n\n", now);

    free(now);

    return 0;
}
