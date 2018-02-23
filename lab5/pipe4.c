#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int fd;
    char *fifoFile = "/tmp/fifoFile";
    char string[] = "The truth is out there!!!";

    // Create the fifo (names pipe)
    mkfifo(fifoFile, 0666);

    fd = open(fifoFile, O_WRONLY);
    write(fd, string, sizeof(string));
    close(fd);

    unlink(fifoFile);

    return 0;
}
