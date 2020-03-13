#include "bigint10.h"
#include "soyccan.h"

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