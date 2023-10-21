#include <windows.h>

#include "time.h"

int get_time_ms() {
    return GetTickCount();
}