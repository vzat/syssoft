#include <stdio.h>
#include <stdlib.h>

#include "timelib.h"
#include "backupAndTransfer.h"

void scheduleBackupAndTransfer(int hour, int min, int sec) {

    while (1) {
        // Wait for scheduled time
        waitForTime(hour, min, sec);

        // Call backup & transfer
        backupAndTransfer();
    }
}
