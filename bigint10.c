#include "bigint10.h"

#include "soyccan.h"

// static char buf[0x500];

static inline void __bigint_inverse_array(BigInt* x)
{
    x->neg = !x->neg;
    for (size_t i = 0; i < x->n; i++)
        x->a[i] = -x->a[i];
}

static inline void __bigint_grow(BigInt* x)
{
    x->cap *= 2;
    x->a = realloc(x->a, x->cap * sizeof(x->a[0]));
}

static void __bigint_carry(BigInt* x)
{
    // DBG("before carry : "); PRINT_BIGINT(x);
    if (x->n == 0)
        return;
    while (x->n > 1 && x->a[x->n - 1] == 0)
        x->n--;
    x->a[x->n] = 0;
    for (size_t i = 0; i < x->n; i++) {
        bool b = x->a[i] < 0 && x->a[i] % 10 != 0;
        x->a[i + 1] += x->a[i] / 10 - b;
        x->a[i] = x->a[i] % 10 + b * 10;
    }
    if (x->a[x->n] != 0) {
        x->n++;
        if (x->n >= x->cap)
            __bigint_grow(x);
    }
    if (x->a[x->n - 1] < 0) {
        __bigint_inverse_array(x);
        __bigint_carry(x);
    }
    assert(x->a[x->n] >= 0);
    while (x->n > 1 && x->a[x->n - 1] == 0)
        x->n--;
    if (x->n == 1 && x->a[0] == 0)
        x->neg = false;  // 0 is seen positive
    // DBG("after carry : "); PRINT_BIGINT(x);
}

void bigint_init(BigInt* x)
{
    if (!x->a) {
        x->cap = BIGINT_INIT_CAPACITY;
        x->a = malloc(x->cap * sizeof(signed char));
    }
    x->n = 0;
    x->neg = false;
}

void bigint_free(BigInt* x)
{
    if (x->a)
        free(x->a);
}

BigInt* bigint_copy(BigInt* dest, const BigInt* src)
{
    // TODO: dest don't have to be src->cap large, but rather src->n
    dest->a = realloc(dest->a, src->cap * sizeof(src->a[0]));
    memcpy(dest->a, src->a, src->n * sizeof(src->a[0]));
    dest->n = src->n;
    dest->cap = src->cap;
    dest->neg = src->neg;
    return dest;
}

static BigInt* __bigint_iadd(BigInt* x, const BigInt* y)
{
    size_t i;
    for (i = 0; i < x->n || i < y->n; i++) {
        if (i >= x->cap)
            __bigint_grow(x);
        assert(i < x->cap);

        if (i < x->n && i < y->n)
            x->a[i] += y->a[i];
        else if (i < y->n)
            x->a[i] = y->a[i];
    }
    x->n = i;
    __bigint_carry(x);
    return x;
}

static BigInt* __bigint_isub(BigInt* x, const BigInt* y)
{
    size_t i;
    for (i = 0; i < x->n || i < y->n; i++) {
        if (i >= x->cap)
            __bigint_grow(x);
        assert(i < x->cap);

        if (i < x->n && i < y->n)
            x->a[i] -= y->a[i];
        else if (i < y->n)
            x->a[i] = -y->a[i];
    }
    x->n = i;
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
    z->n = x->n + y->n + 1;
    while (z->n >= z->cap)
        __bigint_grow(z);
    for (size_t i = 0; i < z->n; i++) {
        z->a[i] = 0;
    }
    for (size_t i = 0; i < x->n; i++) {
        for (size_t j = 0; j < y->n; j++) {
            signed char t = x->a[i] * y->a[j];
            z->a[i + j] += t % 10;
            z->a[i + j + 1] += t / 10;
        }
    }
    __bigint_carry(z);
    return z;
}

BigInt* bigint_idiv2(BigInt* x)
{
    if (x->n == 0)
        return x;
    for (size_t i = x->n - 1; i > 0; i--) {
        x->a[i - 1] += 10 * (x->a[i] % 2);
        x->a[i] /= 2;
    }
    x->a[0] /= 2;
    __bigint_carry(x);
    return x;
}

BigInt* bigint_parsestr(const char* str, size_t len, BigInt* x)
{
    bigint_init(x);
    while (len > 0 && (str[len] < '0' || str[len] > '9'))
        len--;
    while (len > 0 && str[len] >= '0' && str[len] <= '9') {
        x->a[x->n++] = str[len--] - '0';
        if (x->n >= x->cap)
            __bigint_grow(x);
    }
    if (len >= 0 && str[len] >= '0' && str[len] <= '9')
        x->a[x->n++] = str[len--] - '0';
    if (len >= 0 && str[len] == '-')
        x->neg = true;
    return x;
}

char* bigint_tostr(const BigInt* x, char* str)
{
    char* s = str;
    if (x->n > 0) {
        if (x->neg)
            *s++ = '-';
        for (size_t i = x->n - 1; i > 0; i--) {
            *s++ = '0' + x->a[i];
        }
        *s++ = '0' + x->a[0];
    }
    *s = '\0';
    return str;
}

/* empty is also zero */
bool bigint_iszero(const BigInt* x)
{
    for (size_t i = 0; i < x->n; i++)
        if (x->a[i] != 0)
            return false;
    return true;
}

bool bigint_less_than(const BigInt* x, const BigInt* y)
{
    if (x->n != y->n)
        return x->n < y->n;
    for (size_t i = x->n - 1; i > 0; i--)
        if (x->a[i] != y->a[i])
            return x->a[i] < y->a[i];
    return x->a[0] < y->a[0];
}

BigInt* bigint_gcd(const BigInt* x, const BigInt* y, BigInt* z)
{
    BIGINT(a);
    BIGINT(b);
    BIGINT(ans);
    bigint_copy(&a, x);
    bigint_copy(&b, y);
    ans.a[0] = 1;
    ans.n = 1;

    BigInt *n, *m;

    if (bigint_less_than(&a, &b)) {
        n = &a;
        m = &b;
    } else {
        n = &b;
        m = &a;
    }
    while (!bigint_iszero(n) && !bigint_iszero(m)) {
        // DBG("\n:::: n=%s m=%s ans=%s", bigint_tostr(n, buf),
        //     bigint_tostr(m, buf + 0x100), bigint_tostr(&ans, buf + 0x200));
        if (n->a[0] % 2 == 0 && m->a[0] % 2 == 0) {
            bigint_iadd(&ans, &ans);
            // DBG("ans*=2 >> n=%s m=%s ans=%s", bigint_tostr(n, buf),
            //     bigint_tostr(m, buf + 0x100), bigint_tostr(&ans, buf + 0x200));
        }
        if (n->a[0] % 2 == 0) {
            bigint_idiv2(n);
            // DBG("n/=2 >> n=%s m=%s ans=%s", bigint_tostr(n, buf),
            //     bigint_tostr(m, buf + 0x100), bigint_tostr(&ans, buf + 0x200));
        }
        if (m->a[0] % 2 == 0) {
            bigint_idiv2(m);
            // DBG("m/=2 >> n=%s m=%s ans=%s", bigint_tostr(n, buf),
            //     bigint_tostr(m, buf + 0x100), bigint_tostr(&ans, buf + 0x200));
        }
        if (bigint_less_than(m, n)) {
            swap(n, m);
            // DBG("swap >> n=%s m=%s ans=%s", bigint_tostr(n, buf),
            //     bigint_tostr(m, buf + 0x100), bigint_tostr(&ans, buf + 0x200));
        }
        bigint_isub(m, n);
        // DBG("m-=n >> n=%s m=%s ans=%s", bigint_tostr(n, buf),
        //     bigint_tostr(m, buf + 0x100), bigint_tostr(&ans, buf + 0x200));
    }
    bigint_mul(n, &ans, z);
    // DBG("n * ans >> n=%s m=%s ans=%s z=%s", bigint_tostr(n, buf),
    //     bigint_tostr(m, buf + 0x100), bigint_tostr(&ans, buf + 0x200),
    //     bigint_tostr(z, buf + 0x300));
    bigint_free(&a);
    bigint_free(&b);
    bigint_free(&ans);
    return z;
}