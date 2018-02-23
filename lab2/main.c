#include <stdio.h>
#include <math.h>

int main( int argc, char **argv )  {
   int aor;

   printf("%s\n\n", argv[1]);

   aor = areaOfRectangle(10, 80);
   
   printf("Area of Rectangle: %d\n", aor);

   return 0;
}
