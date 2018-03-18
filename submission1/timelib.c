#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "macros.h"

void setCurrentTime () {
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

void getCurrentTime (char * nowString, size_t stringSize) {
    FILE * fp;
    fp = fopen(LAST_LOG_TIME_PATH, "r");
    fread(nowString, stringSize, 1, fp);
    fclose(fp);
}

void getCurrentTimeFileNameSafe (char * nowString, size_t stringSize) {
    time_t now;
    struct tm * nowInfo;

    time(&now);
    nowInfo = localtime(&now);

    memset(nowString, 0, stringSize);
    strftime(nowString, stringSize, "%d-%m-%yT%H-%M-%S", nowInfo);
}

void waitForTime (int hour, int min, int sec, char * message, size_t messageSize) {
    mqd_t mq;

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

    char * buffer = (char *) malloc(MAX_BUF);
    sprintf(buffer, "<info> timelib: The task \"%s\" is scheduled to run at %s", message, nowString);

    // Send log message
    mq = mq_open(QUEUE_NAME, O_WRONLY);
    mq_send(mq, buffer, strlen(buffer), 0);
    mq_close(mq);

    free(nowString);
    free(buffer);

    // Loop until time is reached
    do {
        sleep(1);
        time(&now);
        seconds = difftime(now, mktime(triggerTime));
    } while (seconds < 0);

    sleep(1);

    buffer = (char *) malloc(MAX_BUF);
    sprintf(buffer, "<info> timelib: The scheduled task \"%s\" will now run (TIME-REACHED)", message);

    mq = mq_open(QUEUE_NAME, O_WRONLY);
    mq_send(mq, message, messageSize, 0);
    mq_send(mq, buffer, strlen(buffer), 0);
    mq_close(mq);

    free(buffer);

    exit(EXIT_SUCCESS);
}
