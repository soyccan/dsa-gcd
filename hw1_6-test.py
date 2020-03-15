import math, random

def gcd_binary(n, m):
    if n > m:
        n,m=m,n
    ans=1
    while n >0 and m>0:
        print('\n::: n=%d m=%d ans=%d'%(n,m,ans))
        if n%2==0 and m%2==0:
            ans*=2
        if n%2==0:
            n/=2
        if m%2==0:
            m/=2
        if n>m: n,m=m,n
        m-=n
    return ans*n


inf = open("hw1_6.in", 'w')
ouf = open('hw1_6.out', 'w')
for _ in range(1):
    g = random.randint(10**228, 10**229)
    a = random.randint(10**26, 10**27)
    b = random.randint(10**26, 10**27)
    print(g*a,g*b,file=inf)
    print(math.gcd(g*a,g*b), file=ouf)
    gcd_binary(g*a, g*b)
