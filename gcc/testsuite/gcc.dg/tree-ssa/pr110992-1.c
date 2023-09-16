/* { dg-do compile } */
/* { dg-options "-O3 -fdump-tree-optimized" } */
static unsigned b;
static short c = 4;
void foo(void);
static short(a)(short d, short g) { return d * g; }
void e();
static char f() {
  b = 0;
  return 0;
}
int main() {
  int h = b;
  if ((short)(a(c && e, 65535) & h)) {
    foo();
    h || f();
  }
}

/* There should be no calls to foo left. */
/* { dg-final { scan-tree-dump-not " foo " "optimized" } } */
