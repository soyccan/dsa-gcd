#include <stdint.h>

#include "soyccan.h"

#define BIGINT_INIT_SIZE 0x100

typedef struct {
    uint64_t* __arr;
    size_t __len;
} BigInt;

void bigint_init(BigInt* a)
{
    a->__len = BIGINT_INIT_SIZE;
    a->__arr = malloc(BIGINT_INIT_SIZE * sizeof(a->__arr[0]));
}

void bigint_grow(BigInt* a)
{
    a->__len *= 2;
    a->__arr = realloc(a->__arr, a->__len * sizeof(a->__arr[0]));
}

void bigint_add(const BigInt* a, const BigInt* b, BigInt* c)
{
    if (!c->__arr) {
        bigint_init(c);
    }
    uint64_t* x = a->__arr;
    uint64_t* y = b->__arr;
    size_t idx = 0;
    while (*x || *y) {
        c->__arr[idx] = *x + *y;
        idx++;
        if (idx > c->__len)
            bigint_grow(c);
        if (*x)
            x++;
        if (*y)
            y++;
    }
}

void bigint_parsestr(const char* str, BigInt* a)
{
    uint64_t x = 0;
    size_t idx = 0;
    while (*str && (*str < '0' || *str > '9'))
        str++;
    while (*str && *str >= '0' && *str <= '9') {
        uint64_t overflow =
            ((x >> 60) + (x >> 62) +
             (((x << 3 & 0x7fffffffffffffff) + (x << 1 & 0x7fffffffffffffff)) >>
              63)) >>
            1;
        uint64_t remainder = *str - '0' + x * 10;
        if (overflow) {
            a->__arr[idx++] = remainder;
            x = overflow;
        } else {
            x = remainder;
        }
        str++;
    }
}

void bigint_tostr(const BigInt* a, char* str)
{
    while () {
        *str = '0' + a->__arr[idx] % 10;
        str++;
    }
}

BigInt* gcd_by_binary(const BigInt* a,
                      const BigInt* b,
                      BigInt* gcd,
                      int* iter_count)
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
            swap(n, m);
        }
        m -= n;
    }
    return n * ans;
}

static void test_add() {}

static void __test()
{
    srand(time(0));
    FOR(i, 0, 1000)
    {
        int a = rand() % 1000000 + 1;
        int b = rand() % 1000000 + 1;
        printf("test round %d, a=%d b=%d\n", i, a, b);
        int _;
        int r[4];
        r[0] = gcd_by_def(a, b, &_);
        r[1] = gcd_by_reverse_search(a, b, &_);
        r[2] = gcd_by_binary(a, b, &_);
        r[3] = gcd_by_euclid(a, b, &_);
        FOR(i, 1, 4) assert(r[i] == r[i - 1]);
    }
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