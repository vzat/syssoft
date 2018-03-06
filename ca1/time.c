#include <stdlib.h>
#include <unistd.h>
#include <time.h>

char * getCurrentDate() {
    time_t now;
    struct tm nowInfo;
    char nowString[20];

    time(&now);
    nowInfo = *localtime(&now);

    strftime(nowString, sizeof(nowString), "%x", nowInfo);

    return nowString;
}
