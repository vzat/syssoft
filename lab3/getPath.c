#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>

// List of functions
int file_select();
extern int alphasort();
char *getcwd(char *buf, size_t size);

int main() {
   int count, i;
   struct direct **files;

   char pathname[MAXPATHLEN];
   int t = 1;
   getcwd(pathname, sizeof(pathname));
   printf("PWD = %s\n",pathname);
   count = scandir(pathname, &files, file_select, alphasort);

   /* No files in Dir */
   if (count <= 0)
   {
       printf("No files in Dir\n");
       exit(0);
   }
   printf("Number of files = %d\n",count);

   for (i=1;i<count+1;++i)
   {
      printf("\n%s",files[i-1]->d_name);
   }

   printf("\n");
}

int file_select(struct direct *entry)
{
 return (1);
}
