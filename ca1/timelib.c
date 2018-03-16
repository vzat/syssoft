#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "macros.h"

void getCurrentTime (char * nowString, size_t stringSize) {
    // time_t now;
    // struct tm * nowInfo;
    //
    // time(&now);
    // nowInfo = localtime(&now);
    //
    // // memset(nowString, 0, stringSize);
    // // strftime(nowString, stringSize, "%d/%m/%y %H:%M:%S", nowInfo);
    //
    //
    // char * tempString = malloc(stringSize);
    // strftime(tempString, stringSize, "%d/%m/%y %H:%M:%S", nowInfo);
    //
    //
    // syslog(LOG_INFO, "---> %s %s", nowString, tempString);
    //
    //
    // // strcpy(nowString, tempString);
    // memcpy(nowString, tempString, stringSize);
    //
    // syslog(LOG_INFO, "------> %s %s", nowString, tempString);
    //
    // free(tempString);

    // int fd;
    // char * fifoFile = FIFO_PATH;
    // fd = open(fifoFile, O_RDONLY);
    // read(fd, nowString, stringSize);
    // close(fd);

    FILE * fp;
    fp = fopen(LAST_LOG_TIME_PATH, "r");
    fread(nowString, stringSize, 1, fp);
    fclose(fp);
}

void setCurrentTime () {
    // time_t now;
    // struct tm * nowInfo;
    //
    // time(&now);
    // nowInfo = localtime(&now);
    //
    // char * fifoFile = FIFO_PATH;
    // mkfifo(fifoFile, 0666);
    //
    // int fd = open(fifoFile, O_WRONLY);
    //
    // size_t max_date = sizeof(char) * MAX_DATE;
    // char * timeString = malloc(max_date);
    // strftime(timeString, max_date, "%d/%m/%y %H:%M:%S\n", nowInfo);
    // write(fd, timeString, strlen(timeString));
    //
    // close(fd);
    // free(timeString);

    time_t now;
    struct tm * nowInfo;

    time(&now);
    nowInfo = localtime(&now);

    FILE * fp;
    fp = fopen(LAST_LOG_TIME_PATH, "w");

    size_t max_date = sizeof(char) * MAX_DATE;
    char * timeString = malloc(max_date);
    strftime(timeString, max_date, "%d/%m/%y %H:%M:%S", nowInfo);
    fwrite(timeString, strlen(timeString), 1, fp);

    fclose(fp);
    free(timeString);
}

void getCurrentTimeFileNameSafe (char * nowString, size_t stringSize) {
    time_t now;
    struct tm * nowInfo;

    time(&now);
    nowInfo = localtime(&now);

    memset(nowString, 0, stringSize);
    strftime(nowString, stringSize, "%d-%m-%yT%H-%M-%S", nowInfo);
}

void waitForTime (int hour, int min, int sec) {
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
