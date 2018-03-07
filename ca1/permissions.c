#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "macros.h"

void blockAccess() {
    printf("Blocking access to files\n\n");

    execlp("chmod", "chmod", "000", INTRANET, LIVE, BACKUP, NULL);

    perror("Cannot change permissions");
    exit(1);
}

void unblockAccess() {
    printf("Unblock access to files\n\n");

    execlp("chmod", "chmod", "777", INTRANET, BACKUP, NULL);

    perror("Cannot change permissions");
    exit(1);
}
