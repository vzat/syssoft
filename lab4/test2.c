#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main () {
  pid_t child_pid = fork();

  if (child_pid > 0) {
      // Parent process
      sleep(10);
  }
  else {
      // Child Process
      exit(1);
  }
}
