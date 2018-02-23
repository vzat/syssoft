#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char **argv) {

  struct dirent *entry;

  DIR *dir = opendir(".");

  if (dir == NULL) {
      printf("Could not open the current directory");
      return 127;
  }

  while ((entry = readdir(dir)) != NULL) {
      char *filename = entry->d_name;

      if (strcmp(filename, ".") && strcmp(filename, "..")) {
          struct stat ret;

          stat(filename, &ret);
          mode_t perm = ret.st_mode;

          if (perm & S_IRUSR) {
              printf("r");
          }
          else {
              printf("-");
          }

          if (perm & S_IWUSR) {
              printf("w");
          }
          else {
              printf("-");
          }

          if (perm & S_IXUSR) {
              printf("x");
          }
          else {
              printf("-");
          }

          printf("\t");

          printf("%s\n", entry->d_name);
      }
  }

  closedir(dir);

  return 0;
}
