#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char **argv) {
    char *args[] = {"ls", "-l", NULL};

    printf("\nls -l\n\n");
    execv("/bin/ls", args);

    return 0;
}
