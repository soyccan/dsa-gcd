#include "soyccan.h"

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
#define PRINT_BIGINT(x)                     \
    {                                       \
        fprintf(stderr, "neg %d; ", (x)->neg);       \
        for (size_t i = 0; i < (x)->len; i++) \
            fprintf(stderr, "%hhd ", (x)->__arr[i]);     \
        fprintf(stderr,"\n");                           \
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