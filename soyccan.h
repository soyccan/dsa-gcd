#ifndef _SOYCCAN_H
#define _SOYCCAN_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NDEBUG
#define DBG(format, ...) fprintf(stderr, format "\n", ##__VA_ARGS__)
#define INFILE() freopen("in.txt", "r", stdin)
#else
#define DBG(...)
#define INFILE()
#endif

#ifndef maximize
#define maximize(target, value)         \
    {                                   \
        typeof(value) _v = (value);     \
        typeof(target)* _t = &(target); \
        if (*_t < _v)                   \
            *_t = _v;                   \
    }
#endif

#ifndef minimize
#define minimize(target, value)         \
    {                                   \
        typeof(value) _v = (value);     \
        typeof(target)* _t = &(target); \
        if (*_t > _v)                   \
            *_t = _v;                   \
    }
#endif

#ifndef swap
#define swap(x, y)          \
    {                       \
        typeof(x) _t = (x); \
        (x) = (y);          \
        (y) = _t;           \
    }
#endif

#ifndef FOR
#define FOR(i, start, end) for (typeof(start) i = start; i < end; ++i)
#endif

// min/max from linux-5.5/tools/include/linux/kernel.h
#ifndef max
#define max(x, y)                      \
    ({                                 \
        typeof(x) _max1 = (x);         \
        typeof(y) _max2 = (y);         \
        (void) (&_max1 == &_max2);     \
        _max1 > _max2 ? _max1 : _max2; \
    })
#endif

#ifndef min
#define min(x, y)                      \
    ({                                 \
        typeof(x) _min1 = (x);         \
        typeof(y) _min2 = (y);         \
        (void) (&_min1 == &_min2);     \
        _min1 < _min2 ? _min1 : _min2; \
    })
#endif

#endif
