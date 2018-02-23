#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  int pid = fork();

  if (pid > 0) {
      printf("\nParent Process\n\n");
      sleep(10);
      exit(EXIT_SUCCESS);
  }
  else if (pid == 0) {
      printf("\nChild Process\n\n");

      while (1) {
          sleep(1);
          printf("child pid: %i\n", getpid());
          printf("child ppid: %i\n\n", getppid());
      }
  }

  return 0;
}
