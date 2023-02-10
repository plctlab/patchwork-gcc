/* { dg-do compile } */
/* { dg-options "-Os -mstrict-align" } */

int a[2*3];
int f(int t)
{
  __builtin_memset(a, t, 2*3*sizeof(int));
}

/* memset (a, (val), sizeof = 2*3*4) should be expanded out
   such that there are no overlap stores when -mstrict-align
   is in use. As the alignment of a is 4 byte aligned (due to -Os),
   store word pairs are needed. so 3 stp are in use.  */

/* { dg-final { scan-assembler-times "stp\ts" 3 } } */
