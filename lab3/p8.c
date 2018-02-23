#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

void printPerm(mode_t perm) {
    // Owner
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

    // Group
    if (perm & S_IRGRP) {
        printf("r");
    }
    else {
        printf("-");
    }

    if (perm & S_IWGRP) {
        printf("w");
    }
    else {
        printf("-");
    }

    if (perm & S_IXGRP) {
        printf("x");
    }
    else {
        printf("-");
    }

    // Other
    if (perm & S_IROTH) {
        printf("r");
    }
    else {
        printf("-");
    }

    if (perm & S_IWOTH) {
        printf("w");
    }
    else {
        printf("-");
    }

    if (perm & S_IXOTH) {
        printf("x");
    }
    else {
        printf("-");
    }
}

int main(int argc, char **argv) {
  char filetype[15];

  if (argc == 2) {
      strcpy(filetype, argv[1]);
  }
  else {
      strcpy(filetype, "");
  }

  struct dirent *entry;

  DIR *dir = opendir(".");

  if (dir == NULL) {
      printf("Could not open the current directory");
      return -1;
  }

  while ((entry = readdir(dir)) != NULL) {
      char *filename = entry->d_name;

      if (strcmp(filename, ".") && strcmp(filename, "..") && strstr(filename, filetype) != NULL) {
          struct stat ret;

          stat(filename, &ret);
          mode_t perm = ret.st_mode;

          printPerm(perm);

          printf("\t");

          printf("%s\n", entry->d_name);
      }
  }

  closedir(dir);

  return 0;
}
