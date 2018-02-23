#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {

    char *romanYear;
    int year = 0;
    // char romanNumerals[] = ["I", "IV", "V", "IX", "X", "XL", "L", "XC", "C", "CD", "D", "CM", "M"];
    // int numerals[] = [1, 4, 5, 9, 10, 40, 50, 90, 100, 400, 500, 900, 1000];

    char *romanNumerals[13] = {"CM", "CD", "XC", "XL", "IX", "IV", "M", "D", "C", "L", "X", "V", "I"};
    int numerals[13] = {900, 400, 90, 40, 9, 4, 1000, 500, 100, 50, 10, 5, 1};

    if (argc != 2) {
        printf("This program takes exactly 1 argument\n");
    }
    else {
        romanYear = argv[1];

        char *pos;
        for (int i = 0 ; i < 13 ; i++) {
            while ( pos = strstr(romanYear, romanNumerals[i])) {
                year += numerals[i];

                int numeralSize = strlen(romanNumerals[i]);

                // dest, source, size
                memmove(pos, pos + numeralSize, strlen(pos) - numeralSize + 1);
            }
        }

        printf("Year: %d\n", year);
    }

    return 0;
}
