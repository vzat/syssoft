#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#include "macros.h"

void initAudit() {
    // auditd
}

void logChanges(char * lastLogTime) {
    // ausearch from last log time (size of MAX_DATE)
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
