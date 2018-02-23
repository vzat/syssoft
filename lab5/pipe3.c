#include <stdio.h>

int main() {
    FILE *fp;
    int status;
    char path[1024];

    fp = popen("ls *", "r");

    while (fgets(path, sizeof(path), fp) != NULL) {
        printf("%s", path);
    }

    status = pclose(fp);

    return 0;
}
