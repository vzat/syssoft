#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main() {
  int pid = fork();

  if (pid > 0) {
      printf("\nParent Process\n\n");
      sleep(10);
      exit(EXIT_SUCCESS);
  }
  else if (pid == 0) {
      printf("\nChild Process\n\n");

      // Elavate orphan to session leader
      if (setsid() < 0) {
          exit(EXIT_FAILURE);
      }

      // Set file mode creation to 0
      umask(0);

      // Change working dir to /
      if (chdir("/") < 0) {
          exit(EXIT_FAILURE);
      }

      // Close all file descriptors
      for (int x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
          close(x);
      }

      while (1) {
          sleep(1);
          printf("child pid: %i\n", getpid());
          printf("child ppid: %i\n\n", getppid());
      }
  }

  return 0;
}
