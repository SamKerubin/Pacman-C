#ifndef TIME_MS_H
#define TIME_MS_H

#include <time.h>
#include <stdint.h>

static inline int64_t get_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000LL) + (ts.tv_nsec / 1000000LL);
}

#endif // !TIME_MS_H
