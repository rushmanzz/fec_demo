#include "util.h"
#include <sys/time.h>

uint32_t currentMs() {
    struct timeval time;
    ::gettimeofday(&time, NULL);
    return uint32_t((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

void itimeofday(long *sec, long *usec) {
    struct timeval time;
    gettimeofday(&time, NULL);
    if (sec) *sec = time.tv_sec;
    if (usec) *usec = time.tv_usec;
}

uint64_t iclock64(void) {
    long s, u;
    uint64_t value;
    itimeofday(&s, &u);
    value = ((uint64_t) s) * 1000 + (u / 1000);
    return value;
}

uint32_t iclock() {
    return (uint32_t) (iclock64() & 0xfffffffful);
}


