#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "timelib.h"

void getCurrentTime(char * nowString, size_t stringSize) {
    time_t now;
    struct tm * nowInfo;

    time(&now);
    nowInfo = localtime(&now);

    strftime(nowString, stringSize, "%d/%m/%Y %H:%M:%S", nowInfo);
}

void waitForTime(int hour, int min, int sec) {
    time_t now;

    struct tm * triggerTime;

    time(&now);

    triggerTime = localtime(&now);
    triggerTime->tm_hour = hour;
    triggerTime->tm_min = min;
    triggerTime->tm_sec = sec;

    double seconds = difftime(now, mktime(triggerTime));

    do {
        sleep(1);
        time(&now);
        seconds = difftime(now, mktime(triggerTime));
    } while (seconds < 0);

    printf("Time Reached\n\n");
}
