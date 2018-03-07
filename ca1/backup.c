#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "macros.h"

void backup() {
    printf("Backing up Live\n\n");

    // Trailing slash for the directories is necessary to copy just the contents!!!
    execlp("rsync", "rsync", "-avP", LIVE, BACKUP, NULL);

    perror("Cannot backup files");
    exit(1);
}
