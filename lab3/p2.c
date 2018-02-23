#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main (int argc, char **argv) {
    switch(argc) {
        case 1:
            printf("\nps aux\n\n");
            char *args[] = {"ps", "aux", NULL};
            execv("/bin/ps", args);

            break;
        case 3:
            if (strcmp(argv[1], "search") == 0) {
                printf("\npgrep -f %s\n\n", argv[2]);
                char *args[] = {"pgrep", "-f", argv[2], NULL};
                execv("/usr/bin/pgrep", args);
            }
            else if (strcmp(argv[1], "kill") == 0) {
                printf("\nkill %s\n\n", argv[2]);
                char *args[] = {"kill", argv[2], NULL};
                execv("/bin/kill", args);
            }

            break;
        default:
            break;
    }

    printf("\nInvalid parameters\n\n");

    return 0;
}
