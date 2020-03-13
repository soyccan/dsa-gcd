#include "bigint10.h"

#include "soyccan.h"

static char buf[0x500];

static gcd(long long a, long long b) {
    if (b == 0) return a;
    return gcd(b, a%b);
}

int main()
{
    srand(time(0));
    long long i = 100000, a, b, c, n;
    BIGINT(x);
    BIGINT(y);
    BIGINT(z);
    while (i--) {
        a = rand() % 2000000000 - 1000000000;
        b = rand() % 2000000000 - 1000000000;
        sprintf(buf, "%d", a);
        bigint_parsestr(buf, strlen(buf), &x);
        sprintf(buf, "%d", b);
        bigint_parsestr(buf, strlen(buf), &y);

        bigint_add(&x, &y, &z);
        bigint_tostr(&z, buf);
        c = atoi(buf);
        DBG("a=%26d b=%26d bigint(a+b)=%26s;%26d a+b=%26d", a, b, buf, c,
            a + b);
        assert(c == a + b);

        bigint_sub(&x, &y, &z);
        bigint_tostr(&z, buf);
        c = atoi(buf);
        DBG("a=%26d b=%26d bigint(a-b)=%26s;%26d a-b=%26d", a, b, buf, c,
            a - b);
        assert(c == a - b);

        bigint_mul(&x, &y, &z);
        bigint_tostr(&z, buf);
        c = strtoll(buf, NULL, 10);
        DBG("a=%26d b=%26d bigint(a*b)=%26s;%26lld a*b=%26lld", a, b, buf, c,
            a * b);
        assert(c == a * b);

        a = abs(a);
        b = abs(b);
        x.neg = y.neg = false;
        bigint_gcd(&x, &y, &z);
        bigint_tostr(&z, buf);
        c = strtoll(buf, NULL, 10);
        DBG("a=%26d b=%26d bigint(gcd)=%26s;%26lld a*b=%26lld", a, b, buf, c,
            gcd(a, b));
        assert(c == gcd(a,b));
    }
    bigint_free(&x);
    bigint_free(&y);
    bigint_free(&z);
    return 0;
}