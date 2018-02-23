#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

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

      time_t now;
      struct tm time1, time2;

      char path[] = "/home/monotux/college/syssoft/lab4/dummy";
      mode_t mode1 = 0777;
      mode_t mode2 = 0000;

      while (1) {
          time(&now);

          time1 = *localtime(&now);
          time2 = *localtime(&now);

          time1.tm_hour = 14;
          time1.tm_min = 5;
          time1.tm_sec = 0;

          time2.tm_hour = 14;
          time2.tm_min = 6;
          time2.tm_sec = 0;

          double secTime1 = difftime(now, mktime(&time1));
          double secTime2 = difftime(now, mktime(&time2));

          if (secTime1 >= 0 && secTime1 < 2) {
              if (chmod(path, mode1) < 0) {
                  printf("Error\n\n");
                  exit(1);
              }
          }

          if (secTime2 >= 0 && secTime2 < 2) {
              if (chmod(path, mode2) < 0) {
                  printf("Error\n\n");
                  exit(1);
              }
          }

          sleep(1);

      }

  }

  return 0;
}
