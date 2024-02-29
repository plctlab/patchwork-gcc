/* { dg-do compile } */
/* { dg-options "-O3 -march=z14 -mzarch --save-temps" } */
/* { dg-do run { target { s390_z14_hw } } } */
/* { dg-final { check-function-bodies "**" "" "" { target { lp64 } } } } */

#include <assert.h>
#include <stdint.h>

/*
** long_double_to_i64:
**	ld	%f0,0\(%r2\)
**	ld	%f2,8\(%r2\)
**	cgxbr	%r2,5,%f0
**	br	%r14
*/
__attribute__ ((noipa)) static int64_t
long_double_to_i64 (long double x)
{
  return x;
}

int
main (void)
{
  assert (long_double_to_i64 (42.L) == 42);
  assert (long_double_to_i64 (-42.L) == -42);
}
