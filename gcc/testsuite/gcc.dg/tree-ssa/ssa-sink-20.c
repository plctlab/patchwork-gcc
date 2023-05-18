/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-sink -fdump-tree-optimized -fdump-tree-sink-stats" } */

void bar();
int j;
void foo(int a, int b, int c, int d, int e, int f)
{
  int l;
  l = a + b + c + d +e + f;
  if (a != 5)
    {
      bar();
      j = l;
    }
}
/* { dg-final { scan-tree-dump-times "Sunk statements: 5" 1 "sink" } } */
