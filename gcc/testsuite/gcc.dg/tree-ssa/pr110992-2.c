/* { dg-do compile } */
/* { dg-options "-O3 -fdump-tree-optimized" } */
static unsigned b;
static short c = 4;
void foo(void);
int main() {
  int h = b;
  int d = c != 0;
  if (h*d) {
    foo();
    if (!h) b = 20;
  }
}


/* There should be no calls to foo left. */
/* { dg-final { scan-tree-dump-not " foo " "optimized" } } */
