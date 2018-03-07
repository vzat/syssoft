#include <stdio.h>
#include <stdlib.h>

#include "timelib.h"

void triggerBackup(int hour, int min, int sec) {

    // Wait for scheduled time
    waitForTime(hour, min, sec);

    // Call backup & transfer

    exit(EXIT_SUCCESS);
}
