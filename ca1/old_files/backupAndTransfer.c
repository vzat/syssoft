#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "permissions.h"
#include "backup.h"
#include "transfer.h"

void backupAndTransfer() {
    int status;

    switch (fork()) {
        case 0:
            blockAccess();
        case -1:
            perror("Error init fork");
            exit(1);
        default:
            break;
    }

    wait(&status);

    switch (fork()) {
        case 0:
            backup();
        case -1:
            perror("Error init fork");
            exit(1);
        default:
            break;
    }

    wait(&status);

    switch (fork()) {
        case 0:
            transfer();
        case -1:
            perror("Error init fork");
            exit(1);
        default:
            break;
    }

    wait(&status);

    switch (fork()) {
        case 0:
            unblockAccess();
        case -1:
            perror("Error init fork");
            exit(1);
        default:
            break;
    }
}
