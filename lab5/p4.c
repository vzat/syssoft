#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

void exec1();
void exec2();
void exec3();

int fd[2], fd1[2];
pid_t pid;

int main() {
    // Create pipe
    if (pipe(fd) == -1) {
          perror("Error init pipe");
          exit(1);
    }

    // Create child process - ls
    if ((pid = fork()) == -1) {
        perror("Error init fork");
        exit(1);
    }
    if (pid == 0) {
        // Create pipe
        if (pipe(fd1) == -1) {
              perror("Error init pipe");
              exit(1);
        }

        // Create child process - awk
        if ((pid = fork()) == -1) {
            perror("Error init fork");
            exit(1);
        }
        if (pid == 0) {
            // Child
            exec1();
        }
        // Child
        exec2();
    }

    // Create child process - sort
    if ((pid = fork()) == -1) {
        perror("Error init fork");
        exit(1);
    }
    if (pid == 0) {
        // Child
        exec3();
    }

    close(fd[0]);
    close(fd[1]);
}

void exec1() {
    // input from stdin
    // output to pipe
    dup2(fd1[1], 1);

    // close fds
    close(fd1[0]);
    close(fd1[1]);

    // exec
    execlp("ls", "ls", "-la", NULL);

    // exec did not work - exit
    perror("Error with ls");
    exit(1);
}

void exec2() {
    // input from pipe
    // output to pipe
    dup2(fd1[0], 0);
    dup2(fd[1], 1);

    // close fds
    close(fd1[0]);
    close(fd1[1]);

    // exec
    execlp("awk", "awk", "{print $9}", NULL);

    // exec did not work - exit
    perror("Error with awk");
    exit(1);
}

void exec3() {
    // input from pipe
    // output to stdout - default
    dup2(fd[0], 0);

    // close fds
    close(fd[0]);
    close(fd[1]);

    // exec
    execlp("sort", "sort", "-u", NULL);

    // exec did not work - exit
    perror("Error with sort");
    exit(1);
}
