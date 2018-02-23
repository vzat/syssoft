#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

void exec1();
void exec2();

int fd[2];
pid_t pid;

int main() {
    // Create pipe
    if (pipe(fd) == -1) {
          perror("Error init pipe");
          exit(1);
    }

    // Create child process - ps aux
    if ((pid = fork()) == -1) {
        perror("Error init fork");
        exit(1);
    }
    if (pid == 0) {
        // Child
        exec1();
    }

    // Create child process - grep login
    if ((pid = fork()) == -1) {
        perror("Error init fork");
        exit(1);
    }
    if (pid == 0) {
        // Child
        exec2();
    }

    close(fd[0]);
    close(fd[1]);
}

void exec1() {
    // input from stdin
    // output to pipe
    dup2(fd[1], 1);

    // close fds
    close(fd[0]);
    close(fd[1]);

    // exec
    execlp("ps", "ps", "aux", NULL);

    // exec did not work - exit
    perror("Error with ps aux");
    exit(1);
}

void exec2() {
    // input from pipe
    // output to stdout - default
    dup2(fd[0], 0);

    // close fds
    close(fd[0]);
    close(fd[1]);

    // exec
    execlp("grep", "grep", "login", NULL);

    // exec did not work - exit
    perror("Error with grep");
    exit(1);
}
