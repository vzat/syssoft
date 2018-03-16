#ifndef TIMELIB_H
#define TIMELIB_H

#include <stdlib.h>

void getCurrentTime(char *, size_t);
void getCurrentTimeFileNameSafe(char *, size_t);
void waitForTime(int, int, int);

#endif  // TIMELIB_H
