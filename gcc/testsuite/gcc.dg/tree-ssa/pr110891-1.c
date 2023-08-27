/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

void foo(void);
static int a, c = 7, d, o, q;
static int *b = &a, *f, *j = &d, *n = &c, *ae;
static short e, m;
static short *i = &e;
static char r;
void __assert_fail(char *, char *, int, const char *) __attribute__((__noreturn__));
static const short g();
static void h();
static int *k(int) {
    (*i)++;
    *j ^= *b;
    return &a;
}
static void l(unsigned p) {
    int *aa = &o;
    h();
    o = 5 ^ g() && p;
    if (f == &d || f == &c || f == &a)
        ;
    else {
        foo();
        __assert_fail("", "", 3, __PRETTY_FUNCTION__);
    }
    *aa ^= *n;
    if (*aa)
        if (!(((p) >= 0) && ((p) <= 0))) {
            __builtin_unreachable();
        }
    k(p);
}
static const short g() { return q; }
static void h() {
    unsigned ag = c;
    d = ag > r ? ag : 0;
    ae = k(c);
    f = ae;
    if (ae == &d || ae == &c || ae == &a)
        ;
    else
        __assert_fail("", "", 4, __PRETTY_FUNCTION__);
}
int main() {
    l(a);
    m || (*b |= 64);
    *b &= 5;
}

/* We should be able to optimize away foo. */
/* { dg-final { scan-tree-dump-not "foo " "optimized" } } */
