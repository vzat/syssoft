#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

void getCurrentTime (char * nowString, size_t stringSize) {
    time_t now;
    struct tm * nowInfo;

    time(&now);
    nowInfo = localtime(&now);

    memset(nowString, 0, stringSize);
    strftime(nowString, stringSize, "%d/%m/%y %H:%M:%S", nowInfo);
}
