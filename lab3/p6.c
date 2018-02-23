#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void sig_handler (int sigNum) {
    if (sigNum == SIGUSR1) {
        printf("SIGUSR1 Received\n");
    }
}

int main (int argc, char **argv) {
    if (signal(SIGUSR1, sig_handler) == SIG_ERR) {
        printf("\nSomething went wrong\n\n");
    }

    pid_t pid = fork();
    int status;

    if (pid == 0) {
        char *args[] = {"chmod", "777", "log.txt", NULL};

        printf("\nchmod 777 log.txt\n\n");

        execv("/bin/chmod", args);

        printf("\nEND CHILD\n\n");
    }
    else if (pid > 0) {
        printf("\nParent process\n\n");

        // Wait for child to finish
        wait(&status);

        printf("\nEND PARENT\n\n");
    }

    return 0;
}
