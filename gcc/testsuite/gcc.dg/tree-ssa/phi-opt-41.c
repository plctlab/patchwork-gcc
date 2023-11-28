/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-phiopt2" } */

int foo1 (int data, int res)
{
  res = data & 0xf;
  res |= res << 4;
  if (res < 0x22)
    return 0x22;
  return res;
}

int foo2 (int data, int res)
{
  res = data & 0xf;
  unsigned int r = res;
  r*=17;
  res = r;
  if (r < 0x22)
    return 0x22;
  return res;
}

/* { dg-final { scan-tree-dump-times "MAX_EXPR" 2 "phiopt2" } } */