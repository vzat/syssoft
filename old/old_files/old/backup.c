#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define INTRANET "/var/www/html/intranet/"
#define LIVE "/var/www/html/live/"
#define BACKUP "/var/www/html/backup/"

void blockAccess() {
    printf("Blocking access to files\n\n");

    execlp("chmod", "chmod", "000", INTRANET, LIVE, BACKUP, NULL);

    perror("Cannot change permissions");
    exit(1);
}

void backup() {
    printf("Backing up Live\n\n");

    // Trailing slash for the directories is necessary to copy just the contents!!!
    execlp("rsync", "rsync", "-avP", LIVE, BACKUP, NULL);

    perror("Cannot backup files");
    exit(1);
}

void transfer() {
    printf("Transfering changes from Intranet to Live\n\n");

    execlp("rsync", "rsync", "-avP", INTRANET, LIVE, NULL);

    perror("Cannot transfer changes to live");
    exit(1);
}

void unblockAccess() {
    printf("Unblock access to files\n\n");

    execlp("chmod", "chmod", "777", INTRANET, BACKUP, NULL);

    perror("Cannot change permissions");
    exit(1);
}

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

int main(void) {
    pid_t pid;
    int status;

    while (1) {
        // Check if it's 2am ---> backup/transfer
        // Check if manual update was triggered ---> backup/transfer

        if ((pid = fork()) == -1) {
            perror("Error init fork");
            exit(1);
        }
        else if (pid == 0) {
            backupAndTransfer();
        }

        wait(&status);

        getchar();
    }

    return 0;
}
