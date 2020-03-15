/* soyccan.h */
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

/* end soyccan.h */


/* bigint10.h */
#define BIGINT_INIT_CAPACITY 0x8

typedef struct {
    signed char* __arr;
    size_t len;
    size_t cap;  // capacity
    bool neg;    // negative
} BigInt;

#define BIGINT(name)                                      \
    BigInt name = {NULL, 0, BIGINT_INIT_CAPACITY, false}; \
    bigint_init(&(name));

#ifndef NDEBUG
#define PRINT_BIGINT(x)                              \
    {                                                \
        fprintf(stderr, "neg %d; ", (x)->neg);       \
        for (size_t i = 0; i < (x)->len; i++)        \
            fprintf(stderr, "%hhd ", (x)->__arr[i]); \
        fprintf(stderr, "\n");                       \
    }
#else
#define PRINT_BIGINT(...)
#endif

void bigint_init(BigInt* x);
void bigint_free(BigInt* x);
BigInt* bigint_copy(BigInt* dest, const BigInt* src);

BigInt* bigint_iadd(BigInt* x, const BigInt* y);
BigInt* bigint_isub(BigInt* x, const BigInt* y);
BigInt* bigint_imul(BigInt* x, const BigInt* y);
BigInt* bigint_idiv2(BigInt* x);

BigInt* bigint_add(const BigInt* x, const BigInt* y, BigInt* z);
BigInt* bigint_sub(const BigInt* x, const BigInt* y, BigInt* z);
BigInt* bigint_mul(const BigInt* x, const BigInt* y, BigInt* z);

BigInt* bigint_parsestr(const char* str, size_t len, BigInt* x);
char* bigint_tostr(const BigInt* x, char* str);

bool bigint_iszero(const BigInt* x);
bool bigint_less_than(const BigInt* x, const BigInt* y);

BigInt* bigint_gcd(const BigInt* x, const BigInt* y, BigInt* z);
/* end bigint10.h */


/* bigint10.c */
static inline void __bigint_inverse_array(BigInt* x)
{
    x->neg = !x->neg;
    for (size_t i = 0; i < x->len; i++)
        x->__arr[i] = -x->__arr[i];
}

static inline void __bigint_grow(BigInt* x)
{
    x->cap *= 2;
    x->__arr = (signed char*)realloc(x->__arr, x->cap * sizeof(x->__arr[0]));
}

static void __bigint_carry(BigInt* x)
{
    // DBG("before carry : "); PRINT_BIGINT(x);
    if (x->len == 0)
        return;
    while (x->len > 1 && x->__arr[x->len - 1] == 0)
        x->len--;
    if (x->len >= x->cap)  // we must reserve an extra entry for...
        __bigint_grow(x);  // ...
    x->__arr[x->len] = 0;  // this line to use.
    for (size_t i = 0; i < x->len; i++) {
        bool b = x->__arr[i] < 0 && x->__arr[i] % 10 != 0;
        x->__arr[i + 1] += x->__arr[i] / 10 - b;
        x->__arr[i] = x->__arr[i] % 10 + b * 10;
    }
    if (x->__arr[x->len] != 0) {
        x->len++;
    }
    if (x->__arr[x->len - 1] < 0) {
        __bigint_inverse_array(x);
        __bigint_carry(x);
    }
    assert(x->__arr[x->len] >= 0);
    while (x->len > 1 && x->__arr[x->len - 1] == 0)
        x->len--;
    if (x->len == 1 && x->__arr[0] == 0)
        x->neg = false;  // 0 is seen positive
    // DBG("after carry : "); PRINT_BIGINT(x);
}

void bigint_init(BigInt* x)
{
    if (!x->__arr) {
        x->cap = BIGINT_INIT_CAPACITY;
        x->__arr = (signed char*)malloc(x->cap * sizeof(signed char));
    }
    x->len = 0;
    x->neg = false;
}

void bigint_free(BigInt* x)
{
    if (x->__arr)
        free(x->__arr);
}

BigInt* bigint_copy(BigInt* dest, const BigInt* src)
{
    // TODO: dest don't have to be src->cap large, but rather src->n
    dest->__arr = (signed char*)realloc(dest->__arr, src->cap * sizeof(src->__arr[0]));
    memcpy(dest->__arr, src->__arr, src->len * sizeof(src->__arr[0]));
    dest->len = src->len;
    dest->cap = src->cap;
    dest->neg = src->neg;
    return dest;
}

static BigInt* __bigint_iadd(BigInt* x, const BigInt* y)
{
    size_t i;
    for (i = 0; i < x->len || i < y->len; i++) {
        if (i >= x->cap)
            __bigint_grow(x);
        assert(i < x->cap);

        if (i < x->len && i < y->len)
            x->__arr[i] += y->__arr[i];
        else if (i < y->len)
            x->__arr[i] = y->__arr[i];
    }
    x->len = i;
    __bigint_carry(x);
    return x;
}

static BigInt* __bigint_isub(BigInt* x, const BigInt* y)
{
    size_t i;
    for (i = 0; i < x->len || i < y->len; i++) {
        if (i >= x->cap)
            __bigint_grow(x);
        assert(i < x->cap);

        if (i < x->len && i < y->len)
            x->__arr[i] -= y->__arr[i];
        else if (i < y->len)
            x->__arr[i] = -y->__arr[i];
    }
    x->len = i;
    __bigint_carry(x);
    return x;
}

BigInt* bigint_iadd(BigInt* x, const BigInt* y)
{
    if (!x->neg && !y->neg)
        return __bigint_iadd(x, y);
    else if (!x->neg && y->neg)
        return __bigint_isub(x, y);
    else if (x->neg && !y->neg)
        return __bigint_isub(x, y);
    else
        return __bigint_iadd(x, y);
}

BigInt* bigint_isub(BigInt* x, const BigInt* y)
{
    if (!x->neg && !y->neg)
        return __bigint_isub(x, y);
    else if (!x->neg && y->neg)
        return __bigint_iadd(x, y);
    else if (x->neg && !y->neg)
        return __bigint_iadd(x, y);
    else
        return __bigint_isub(x, y);
}

BigInt* bigint_imul(BigInt* x, const BigInt* y)
{
    BIGINT(z);
    bigint_mul(x, y, &z);
    bigint_copy(x, &z);
    bigint_free(&z);
    return x;
}

BigInt* bigint_add(const BigInt* x, const BigInt* y, BigInt* z)
{
    bigint_init(z);
    bigint_copy(z, x);
    bigint_iadd(z, y);
    return z;
}

BigInt* bigint_sub(const BigInt* x, const BigInt* y, BigInt* z)
{
    bigint_init(z);
    bigint_copy(z, x);
    bigint_isub(z, y);
    return z;
}

BigInt* bigint_mul(const BigInt* x, const BigInt* y, BigInt* z)
{
    bigint_init(z);
    z->neg = x->neg ^ y->neg;
    z->len = x->len + y->len + 1;
    while (z->len >= z->cap)
        __bigint_grow(z);
    for (size_t i = 0; i < z->len; i++) {
        z->__arr[i] = 0;
    }
    for (size_t i = 0; i < x->len; i++) {
        for (size_t j = 0; j < y->len; j++) {
            // TODO: will this overflow ?
            int t = z->__arr[i + j] + x->__arr[i] * y->__arr[j];
            z->__arr[i + j] = t % 10;
            assert(z->__arr[i + j + 1] <= 127 - t / 10);
            z->__arr[i + j + 1] += t / 10;
        }
    }
    __bigint_carry(z);
    return z;
}

BigInt* bigint_idiv2(BigInt* x)
{
    if (x->len == 0)
        return x;
    for (size_t i = x->len - 1; i > 0; i--) {
        x->__arr[i - 1] += 10 * (x->__arr[i] % 2);
        x->__arr[i] /= 2;
    }
    x->__arr[0] /= 2;
    __bigint_carry(x);
    return x;
}

BigInt* bigint_parsestr(const char* str, size_t len, BigInt* x)
{
    bigint_init(x);
    while (len > 0 && (str[len] < '0' || str[len] > '9'))
        len--;
    while (len > 0 && str[len] >= '0' && str[len] <= '9') {
        if (x->len >= x->cap)
            __bigint_grow(x);
        x->__arr[x->len++] = str[len--] - '0';
    }
    if (str[len] >= '0' && str[len] <= '9') {
        if (x->len >= x->cap)
            __bigint_grow(x);
        x->__arr[x->len++] = str[len] - '0';
    }
    if (str[len] == '-')
        x->neg = true;
    return x;
}

char* bigint_tostr(const BigInt* x, char* str)
{
    char* s = str;
    if (x->len > 0) {
        if (x->neg)
            *s++ = '-';
        for (size_t i = x->len - 1; i > 0; i--) {
            *s++ = '0' + x->__arr[i];
        }
        *s++ = '0' + x->__arr[0];
    }
    *s = '\0';
    return str;
}

/* empty is also zero */
bool bigint_iszero(const BigInt* x)
{
    for (size_t i = 0; i < x->len; i++)
        if (x->__arr[i] != 0)
            return false;
    return true;
}

bool bigint_less_than(const BigInt* x, const BigInt* y)
{
    if (x->len != y->len)
        return x->len < y->len;
    for (size_t i = x->len - 1; i > 0; i--)
        if (x->__arr[i] != y->__arr[i])
            return x->__arr[i] < y->__arr[i];
    return x->__arr[0] < y->__arr[0];
}

BigInt* bigint_gcd(const BigInt* x, const BigInt* y, BigInt* z)
{
    BIGINT(a);
    BIGINT(b);
    BIGINT(ans);
    bigint_copy(&a, x);
    bigint_copy(&b, y);
    ans.__arr[0] = 1;
    ans.len = 1;

    BigInt *n, *m;

    if (bigint_less_than(&a, &b)) {
        n = &a;
        m = &b;
    } else {
        n = &b;
        m = &a;
    }
    while (!bigint_iszero(n) && !bigint_iszero(m)) {
        // DBG("\n:::: n=%s\n m=%s\n ans=%s", bigint_tostr(n, buf),
        //     bigint_tostr(m, buf + 0x200), bigint_tostr(&ans, buf + 0x400));
        if (n->__arr[0] % 2 == 0 && m->__arr[0] % 2 == 0) {
            bigint_iadd(&ans, &ans);
            // DBG("ans*=2 >> n=%s\n m=%s\n ans=%s", bigint_tostr(n, buf),
            //     bigint_tostr(m, buf + 0x200), bigint_tostr(&ans, buf +
            //     0x400));
        }
        if (n->__arr[0] % 2 == 0) {
            bigint_idiv2(n);
            // DBG("n/=2 >> n=%s\n m=%s\n ans=%s", bigint_tostr(n, buf),
            //     bigint_tostr(m, buf + 0x200), bigint_tostr(&ans, buf +
            //     0x400));
        }
        if (m->__arr[0] % 2 == 0) {
            bigint_idiv2(m);
            // DBG("m/=2 >> n=%s\n m=%s\n ans=%s", bigint_tostr(n, buf),
            //     bigint_tostr(m, buf + 0x200), bigint_tostr(&ans, buf +
            //     0x400));
        }
        if (bigint_less_than(m, n)) {
            BigInt* t = n;
            n = m;
            m = t;
            // DBG("swap >> n=%s\n m=%s\n ans=%s", bigint_tostr(n, buf),
            //     bigint_tostr(m, buf + 0x200), bigint_tostr(&ans, buf +
            //     0x400));
        }
        bigint_isub(m, n);
        // DBG("m-=n >> n=%s\n m=%s\n ans=%s", bigint_tostr(n, buf),
        //     bigint_tostr(m, buf + 0x200), bigint_tostr(&ans, buf + 0x400));
    }
    bigint_mul(n, &ans, z);
    // DBG("n * ans >> n=%s\n m=%s\n ans=%s z=%s", bigint_tostr(n, buf),
    //     bigint_tostr(m, buf + 0x200), bigint_tostr(&ans, buf + 0x400),
    //     bigint_tostr(z, buf + 0x600));
    bigint_free(&a);
    bigint_free(&b);
    bigint_free(&ans);
    return z;
}
/* end bigint10.c */


/* hw1_6.c */
static char buf[0x400];

int main()
{
    BIGINT(x);
    BIGINT(y);
    BIGINT(z);
    char *a = buf, *b = buf + 0x200;
    while (scanf("%300s %300s", a, b) == 2) {
        bigint_parsestr(a, strlen(a), &x);
        bigint_parsestr(b, strlen(b), &y);

        // DBG("x=%s y=%s\n", bigint_tostr(&x, a), bigint_tostr(&y, b));
        // PRINT_BIGINT(&x);
        // PRINT_BIGINT(&y);

        bigint_gcd(&x, &y, &z);

        printf("%s\n", bigint_tostr(&z, buf));
    }
    bigint_free(&x);
    bigint_free(&y);
    bigint_free(&z);
    return 0;
}