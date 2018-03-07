#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "macros.h"
#include "timelib.h"

int main(void) {
    // Create orphan to run the scheduler
    // Create orphan for the daemon
    // They should talk using a fifofile

    // size_t dateSize = sizeof(char) * MAX_DATE;
    // char * now = malloc(dateSize);
    //
    // getCurrentTime(now, dateSize);
    // printf("Current Date: %s\n", now);
    //
    // waitForTime(15, 30, 0);
    //
    // getCurrentTime(now, dateSize);
    // printf("Current Date: %s\n", now);
    //
    // free(now);

    return 0;
}
