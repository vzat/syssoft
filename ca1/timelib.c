#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "macros.h"
#include "timelib.h"

void getCurrentTime(char * nowString, size_t stringSize) {
    time_t now;
    struct tm * nowInfo;

    time(&now);
    nowInfo = localtime(&now);

    strftime(nowString, stringSize, "%d/%m/%y %H:%M:%S", nowInfo);
}

void waitForTime(int hour, int min, int sec) {
    time_t now;

    struct tm * triggerTime;

    time(&now);

    // Set the schedule time
    triggerTime = localtime(&now);
    triggerTime->tm_hour = hour;
    triggerTime->tm_min = min;
    triggerTime->tm_sec = sec;

    // Check if it's past the time and increase the day if it is
    double seconds = difftime(now, mktime(triggerTime));
    if (seconds > 0) {
        triggerTime->tm_mday ++;

        // Normalise date
        mktime(triggerTime);
    }

    // Print fhe scheduled time
    size_t stringSize = sizeof(char) * MAX_DATE;
    char * nowString = (char *) malloc(stringSize);
    strftime(nowString, stringSize, "%d/%m/%Y %H:%M:%S", triggerTime);
    printf("Next schedule date: %s\n\n", nowString);
    free(nowString);

    // Loop until time is reached
    do {
        sleep(1);
        time(&now);
        seconds = difftime(now, mktime(triggerTime));
    } while (seconds < 0);

    printf("Time Reached\n\n");
}
