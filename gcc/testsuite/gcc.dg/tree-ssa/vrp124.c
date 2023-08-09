/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

/* Should be optimized to a == -100 */
int g(int a)
{
  if (a == -100 || a >= 0)
    ;
  else
    return 0;
  return a < 0;
}

/* Should optimize to a == 0 */
int f(int a)
{
  if (a == 0 || a > 100)
    ;
  else
    return 0;
  return a < 50;
}

/* Should be optimized to a == 0. */
int f2(int a)
{
  if (a == 0 || a > 100)
    ;
  else
    return 0;
  return a < 100;
}

/* Should optimize to a == 100 */
int f1(int a)
{
  if (a < 0 || a == 100)
    ;
  else
    return 0;
  return a > 50;
}

/* { dg-final { scan-tree-dump-not "goto " "optimized" } } */
