/* Test C23 Checked Integer Arithmetic macros in <stdckdint.h>.  */
/* { dg-do compile { target bitint } } */
/* { dg-options "-std=c2x" } */

#include <stdckdint.h>

int
main ()
{
  _BitInt(32) a;
  int b;
  ckd_add (&a, 1, 1);		/* { dg-error "types used in ckd_add should be integral other than plain char, bool, bit-precise integer or enumerated type" } */
  ckd_sub (&a, 1, 1);		/* { dg-error "types used in ckd_sub should be integral other than plain char, bool, bit-precise integer or enumerated type" } */
  ckd_mul (&a, 1, 1);		/* { dg-error "types used in ckd_mul should be integral other than plain char, bool, bit-precise integer or enumerated type" } */
  ckd_add (&b, 1wb, 1);		/* { dg-error "types used in ckd_add should be integral other than plain char, bool, bit-precise integer or enumerated type" } */
  ckd_sub (&b, 1wb, 1);		/* { dg-error "types used in ckd_sub should be integral other than plain char, bool, bit-precise integer or enumerated type" } */
  ckd_mul (&b, 1wb, 1);		/* { dg-error "types used in ckd_mul should be integral other than plain char, bool, bit-precise integer or enumerated type" } */
  ckd_add (&b, 1, 1wb);		/* { dg-error "types used in ckd_add should be integral other than plain char, bool, bit-precise integer or enumerated type" } */
  ckd_sub (&b, 1, 1wb);		/* { dg-error "types used in ckd_sub should be integral other than plain char, bool, bit-precise integer or enumerated type" } */
  ckd_mul (&b, 1, 1wb);		/* { dg-error "types used in ckd_mul should be integral other than plain char, bool, bit-precise integer or enumerated type" } */
}
