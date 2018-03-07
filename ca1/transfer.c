#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "macros.h"

void transfer() {
    printf("Transfering changes from Intranet to Live\n\n");

    execlp("rsync", "rsync", "-avP", INTRANET, LIVE, NULL);

    perror("Cannot transfer changes to live");
    exit(1);
}
