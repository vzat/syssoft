#include <stdio.h>
#include <stdlib.h>
#include "areaOfTriangle.h"

int main(int argc, char **argv) {

    float base = 1.0f, height = 1.0f;

    if (argc != 3) {
        printf("This program takes exactly 2 arguments\n");
    }
    else {
        base = (float)atof(argv[1]);
        height = (float)atof(argv[2]);

        areaOfTriangle(base, height);
    }

    return 0;
}
