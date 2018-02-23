#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

int main() {
    int fd[2], nBytes;
    pid_t pid;
    char string[] = "Heya\n";
    char readBuffer[100];

    // Create pipe
    pipe(fd);

    // Create child process
    pid = fork();

    if (pid == 0) {
        // Child

        // Take no input
        close(fd[0]);

        // Second output to parent
        write(fd[1], string, strlen(string) + 1);

        exit(0);
    }
    else {
        // Parent

        // Send no output
        close(fd[1]);

        // Get input from pipe
        nBytes = read(fd[0], readBuffer, sizeof(readBuffer));
        printf("Message from Child: %s", readBuffer);
    }
}
