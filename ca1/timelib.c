#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "timelib.h"

void getCurrentDate(char * nowString, size_t stringSize) {
    time_t now;
    struct tm * nowInfo;

    time(&now);
    nowInfo = localtime(&now);

    strftime(nowString, stringSize, "%d/%m/%Y %H:%M:%S", nowInfo);
}
