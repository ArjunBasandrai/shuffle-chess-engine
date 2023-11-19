#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
#define WINDOWS
#include <windows.h>
#elif defined(__APPLE__)
#define MACOS
#include <sys/time.h>
#endif

#include "gettime.h"

int get_time_ms() {
    #ifdef WINDOWS
    return GetTickCount();
    #else
    struct timeval time_value;
    gettimeofday(&time_value, NULL);
    return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
    #endif
}