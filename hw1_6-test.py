import math, random

inf = open("hw1_6.in", 'w')
ouf = open('hw1_6.out', 'w')
for _ in range(10000):
    g = random.randint(10**220, 10**225)
    a = random.randint(10**25, 10**27)
    b = random.randint(10**25, 10**27)
    print(g*a,g*b,file=inf)
    print(math.gcd(g*a,g*b), file=ouf)
