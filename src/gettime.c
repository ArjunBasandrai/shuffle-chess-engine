#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
#ifndef WINDOWS
#define WINDOWS
#include <windows.h>
#endif
#else
#ifndef UNIX
#define UNIX
#include <sys/time.h>
#endif
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