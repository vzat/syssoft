#include <stdio.h>
#include <stdlib.h>
#include "areaOfCircle.h"

int main(int argc, char **argv) {

    float radius = 1.0f;

    if (argc != 2) {
        printf("This program takes exactly 1 argument\n");
    }
    else {
        radius = (float)atof(argv[1]);

        areaOfCircle(radius);
    }

    return 0;
}
