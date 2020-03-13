#include <time.h>

#include "soyccan.h"

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
            swap(n, m);
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