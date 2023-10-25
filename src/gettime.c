#include <windows.h>

#include "gettime.h"

int get_time_ms() {
    return GetTickCount();
}