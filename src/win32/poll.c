#include "pandio.h"
#include "poll.h"
#include "common.h"
#include <windows.h>
#include <stdio.h>

uint64_t pnd_now() {
    FILETIME ft;
    ULARGE_INTEGER li;

    GetSystemTimeAsFileTime(&ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;

    return li.QuadPart / 10000ULL;
}
