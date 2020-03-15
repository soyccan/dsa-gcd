#include <time.h>

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


int gcd_by_def(int a, int b, int* iter_count)
{
    int ans = 1;
    for (int i = 2; i <= min(a, b) / 2; ++i) {
        (*iter_count)++;
        if (a % i == 0 && b % i == 0) {
            ans = i;
        }
    }
    if (a % b == 0)
        ans = b;
    if (b % a == 0)
        ans = a;
    return ans;
}

int gcd_by_reverse_search(int a, int b, int* iter_count)
{
    for (int i = min(a, b); i >= 1; i--) {
        (*iter_count)++;
        if (a % i == 0 && b % i == 0) {
            return i;
        }
    }
    perror("gcd_by_reverse_search failed to terminate");
    return -1;
}

int gcd_by_binary(int a, int b, int* iter_count)
{
    int n = min(a, b);
    int m = max(a, b);
    int ans = 1;
    while (n != 0 && m != 0) {
        (*iter_count)++;
        if (n % 2 == 0 && m % 2 == 0) {
            ans *= 2;
        }
        if (n % 2 == 0) {
            n /= 2;
        }
        if (m % 2 == 0) {
            m /= 2;
        }
        if (n > m) {
            int t = n;
            n = m;
            m = t;
        }
        m -= n;
    }
    return n * ans;
}

int gcd_by_euclid(int a, int b, int* iter_count)
{
    int n = min(a, b);
    int m = max(a, b);
    while (m % n != 0) {
        (*iter_count)++;
        int tmp = n;
        n = m % n;
        m = tmp;
    }
    return n;
}

int main()
{
    int a, b, iter_count;
    while (scanf("%d", &a) == 1 && a > 0) {
        scanf("%d", &b);
        iter_count = 0;
        printf("(Case %d, %d): GCD-By-Def = %d, taking %d iterations\n", a, b,
               gcd_by_def(a, b, &iter_count), iter_count);
        iter_count = 0;
        printf(
            "(Case %d, %d): GCD-By-Reverse-Search = %d, taking %d iterations\n",
            a, b, gcd_by_reverse_search(a, b, &iter_count), iter_count);
        iter_count = 0;
        printf("(Case %d, %d): GCD-By-Binary = %d, taking %d iterations\n", a,
               b, gcd_by_binary(a, b, &iter_count), iter_count);
        iter_count = 0;
        printf("(Case %d, %d): GCD-By-Euclid = %d, taking %d iterations\n", a,
               b, gcd_by_euclid(a, b, &iter_count), iter_count);
    }
    return 0;
}