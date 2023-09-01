/* { dg-do compile } */
/* { dg-options "-O1 -fdump-tree-fre3 -fdump-tree-optimized" } */
/* part of PR tree-optimization/108397 */

long long
foo (unsigned char o)
{
  unsigned long long t1 = -(long long) (o == 0);
  unsigned long long t2 = -(long long) (t1 != 0);
  unsigned long long t3 = -(long long) (t1 <= t2);
  return t3;
}

/* Should be able to optimize this down to just `return -1;` during fre3. */
/* { dg-final { scan-tree-dump "return -1;" "fre3" } } */
/* FRE does not remove all dead statements */
/* { dg-final { scan-tree-dump-not " - " "fre3" { xfail *-*-* } } } */

/* { dg-final { scan-tree-dump "return -1;" "optimized" } } */
/* { dg-final { scan-tree-dump-not " - " "optimized" } } */
