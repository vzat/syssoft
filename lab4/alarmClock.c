#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

void alarmClock() {
    time_t now;
    struct tm newyear;

    time(&now);

    newyear = *localtime(&now);
    newyear.tm_hour = 13;
    newyear.tm_min = 42;
    newyear.tm_sec = 0;

    double seconds = difftime(now, mktime(&newyear));
    while (seconds < 0) {
        printf("%.f\n", seconds);
        sleep(1);
        time(&now);
        seconds = difftime(now, mktime(&newyear));
    }

    printf("Happy new year");
    exit(EXIT_SUCCESS);
}

int main() {
  int pid = fork();

  if (pid > 0) {
      printf("\nParent Process\n\n");
      sleep(5);
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

      alarmClock();
  }

  return 0;
}
