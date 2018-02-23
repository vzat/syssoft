#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    // if (argc != 2) {
    //     exit(1);
    // }

    char path[] = "/home/monotux/college/syssoft/lab4/dummy";
    // char mode[] = "777";
    // mode_t modeCode = strtol(mode, 0, 8);

    // 0777 is an octal
    mode_t modeCode = 0777;

    if (chmod(path, modeCode) < 0) {
        printf("Error\n\n");
        exit(1);
    }

    printf("Successfuly changed permissions\n\n");

    return 0;
}
