#ifndef TIMELIB_H
#define TIMELIB_H

#include <stdlib.h>

void getCurrentTime(char *, size_t);
void setCurrentTime();
void getCurrentTimeFileNameSafe(char *, size_t);
void waitForTime(int, int, int, char *, size_t);

#endif  // TIMELIB_H
