#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define INTRANET "/var/www/html/intranet"
#define LIVE "/var/www/html/live"
#define BACKUP "/var/www/html/backup"

void blockAccess() {
    printf("Blocking access to files\n\n");

    execlp("chmod", "chmod", "000", INTRANET, LIVE, BACKUP, NULL);

    perror("Cannot change permissions");
    exit(1);
}

void backup() {
    printf("Backing up LIVE\n\n");

    execlp("rsync", "rsync", "-avP", LIVE, BACKUP, NULL);

    perror("Cannot backup files");
    exit(1);

    // FILE *fp;
    //
    // fp = open('/var/www/html/liveUpdates.txt', "r");
    //
    // while (fgets(path, sizeof(path), fp) != NULL) {
    //     printf("%s", path);
    // }
}

void unblockAccess() {
    printf("Unblock access to files\n\n");

    execlp("chmod", "chmod", "777", INTRANET, BACKUP, NULL);

    perror("Cannot change permissions");
    exit(1);
}

int main(void) {
    pid_t pid;
    int status;

    if ((pid = fork()) == -1) {
        perror("Error init fork");
        exit(1);
    }
    else if (pid == 0) {
        blockAccess();
    }

    wait(&status);

    if ((pid = fork()) == -1) {
        perror("Error init fork");
        exit(1);
    }
    else if (pid == 0) {
        backup();
    }

    wait(&status);

    if ((pid = fork()) == -1) {
        perror("Error init fork");
        exit(1);
    }
    else if (pid == 0) {
        unblockAccess();
    }

    return 0;
}
