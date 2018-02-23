#include <stdio.h>
#include <stdlib.h>
#include "areaOfRectangle.h"

int main(int argc, char **argv) {

    float side1 = 1.0f, side2 = 1.0f;

    if (argc != 3) {
        printf("This program takes exactly 2 arguments\n");
    }
    else {
        side1 = (float)atof(argv[1]);
        side2 = (float)atof(argv[2]);

        areaOfRectangle(side1, side2);
    }

    return 0;
}
