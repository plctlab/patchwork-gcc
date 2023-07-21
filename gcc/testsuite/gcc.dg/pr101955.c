/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-dse1 -Wno-psabi" } */

typedef int v4si __attribute__((vector_size(4 * sizeof(int))));

__attribute__((noipa)) int
t1 (int x)
{
  return (x << 31) >> 31;
}

__attribute__((noipa)) int
t2 (int x)
{
  int y = x << 31;
  int z = y >> 31;
  return z;
}

__attribute__((noipa)) int
t3 (int x)
{
  int w = 31;
  int y = x << w;
  int z = y >> w;
  return z;
}

__attribute__((noipa)) long long
t4 (long long x)
{
  return (x << 63) >> 63;
}

__attribute__((noipa)) long long
t5 (long long x)
{
  long long y = x << 63;
  long long z = y >> 63;
  return z;
}

__attribute__((noipa)) long long
t6 (long long x)
{
  int w = 63;
  long long y = x << w;
  long long z = y >> w;
  return z;
}

__attribute__((noipa)) v4si
t7 (v4si x)
{
  return (x << 31) >> 31;
}

__attribute__((noipa)) v4si
t8 (v4si x)
{
  v4si t = {31,31,31,31};
  return (x << t) >> t;
}

/* { dg-final { scan-tree-dump-not " >> " "dse1" } } */
/* { dg-final { scan-tree-dump-not " << " "dse1" } } */
/* { dg-final { scan-tree-dump-times " -" 8 "dse1" } } */
/* { dg-final { scan-tree-dump-times " & " 8 "dse1" } } */

