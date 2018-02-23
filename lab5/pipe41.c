#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_BUF 1024

int main() {
    int fd;
    char *fifoFile = "/tmp/fifoFile";
    char buf[MAX_BUF];

    fd = open(fifoFile, O_RDONLY);
    read(fd, buf, MAX_BUF);

    printf("Message in fifo: %s\n", buf);
    close(fd);

    return 0;
}
