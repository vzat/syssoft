#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "timelib.h"

#define INTRANET "/var/www/html/intranet/"
#define LIVE "/var/www/html/live/"
#define BACKUP "/var/www/html/backup/"
#define AUDIT_KEY "intranet.audit"
#define MAX_DATE 1024

int main(void) {
    size_t dateSize = sizeof(char) * MAX_DATE;
    char * now = malloc(dateSize);
    getCurrentDate(now, dateSize);

    printf("Current Date: %s\n", now);

    free(now);

    return 0;
}
