/* { dg-do compile { target lp64 } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } } */
/* { dg-options "-O2 -mdejagnu-cpu=power7" } */

/* Functional test of the one operand vector builtins.  */

#include <altivec.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0

void abort (void);

int
main () {
  int i;
  vector float f_src = { 125.44, 23.04, -338.56, 17.64};
  vector float f_result;
  vector float f_abs_expected = { 125.44, 23.04, 338.56, 17.64};
  vector float f_ceil_expected = { 126.0, 24.0, -338, 18.0};
  vector float f_floor_expected = { 125.0, 23.0, -339, 17.0};
  vector float f_nearbyint_expected = { 125.0, 23.0, -339, 18.0};
  vector float f_rint_expected = { 125.0, 23.0, -339, 18.0};
  vector float f_sqrt_expected = { 11.2, 4.8, 18.4, 4.2};
  vector float f_trunc_expected = { 125.0, 23.0, -338, 17};

  vector double d_src = { 125.44, -338.56};
  vector double d_result;
  vector double d_abs_expected = { 125.44, 338.56};
  vector double d_ceil_expected = { 126.0, -338.0};
  vector double d_floor_expected = { 125.0, -339.0};
  vector double d_nearbyint_expected = { 125.0, -339.0};
  vector double d_rint_expected = { 125.0, -339.0};
  vector double d_sqrt_expected = { 11.2, 18.4};
  vector double d_trunc_expected = { 125.0, -338.0};

  /* Abs, float */
  f_result = vec_abs (f_src);

  if ((f_result[0] != f_abs_expected[0])
      || (f_result[1] != f_abs_expected[1])
      || (f_result[2] != f_abs_expected[2])
      || (f_result[3] != f_abs_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_abs (float) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     f_abs_expected[0], f_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     f_abs_expected[1], f_result[1]);
      printf("   expected[2] = %f; result[2] = %f\n",
	     f_abs_expected[2], f_result[2]);
      printf("   expected[3] = %f; result[3] = %f\n",
	     f_abs_expected[3], f_result[3]);
    }
#else
  abort();
#endif

  /* Ceiling, float */
  f_result = vec_ceil (f_src);

  if ((f_result[0] != f_ceil_expected[0])
      || (f_result[1] != f_ceil_expected[1])
      || (f_result[2] != f_ceil_expected[2])
      || (f_result[3] != f_ceil_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_ceil (float) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     f_ceil_expected[0], f_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     f_ceil_expected[1], f_result[1]);
      printf("   expected[2] = %f; result[2] = %f\n",
	     f_ceil_expected[2], f_result[2]);
      printf("   expected[3] = %f; result[3] = %f\n",
	     f_ceil_expected[3], f_result[3]);
    }
#else
  abort();
#endif

  /* Floor, float */
  f_result = vec_floor (f_src);

  if ((f_result[0] != f_floor_expected[0])
      || (f_result[1] != f_floor_expected[1])
      || (f_result[2] != f_floor_expected[2])
      || (f_result[3] != f_floor_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_floor (float) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     f_floor_expected[0], f_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     f_floor_expected[1], f_result[1]);
      printf("   expected[2] = %f; result[2] = %f\n",
	     f_floor_expected[2], f_result[2]);
      printf("   expected[3] = %f; result[3] = %f\n",
	     f_floor_expected[3], f_result[3]);
    }
#else
  abort();
#endif

  /* Nearby int, float */
  f_result = vec_nearbyint (f_src);

  if ((f_result[0] != f_nearbyint_expected[0])
      || (f_result[1] != f_nearbyint_expected[1])
      || (f_result[2] != f_nearbyint_expected[2])
      || (f_result[3] != f_nearbyint_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_nearby (float) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     f_nearbyint_expected[0], f_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     f_nearbyint_expected[1], f_result[1]);
      printf("   expected[2] = %f; result[2] = %f\n",
	     f_nearbyint_expected[2], f_result[2]);
      printf("   expected[3] = %f; result[3] = %f\n",
	     f_nearbyint_expected[3], f_result[3]);
    }
#else
  abort();
#endif

  /* Round, float */
  f_result = vec_rint (f_src);

  if ((f_result[0] != f_rint_expected[0])
      || (f_result[1] != f_rint_expected[1])
      || (f_result[2] != f_rint_expected[2])
      || (f_result[3] != f_rint_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_rint (float) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     f_rint_expected[0], f_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     f_rint_expected[1], f_result[1]);
      printf("   expected[2] = %f; result[2] = %f\n",
	     f_rint_expected[2], f_result[2]);
      printf("   expected[3] = %f; result[3] = %f\n",
	     f_rint_expected[3], f_result[3]);
    }
#else
  abort();
#endif

    /* Square, float */
  f_result = vec_sqrt (vec_abs(f_src));

  if ((f_result[0] != f_sqrt_expected[0])
      || (f_result[1] != f_sqrt_expected[1])
      || (f_result[2] != f_sqrt_expected[2])
      || (f_result[3] != f_sqrt_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_sqrt (float) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     f_sqrt_expected[0], f_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     f_sqrt_expected[1], f_result[1]);
      printf("   expected[2] = %f; result[2] = %f\n",
	     f_sqrt_expected[2], f_result[2]);
      printf("   expected[3] = %f; result[3] = %f\n",
	     f_sqrt_expected[3], f_result[3]);
    }
#else
  abort();
#endif

    /* Truncate, float */
  f_result = vec_trunc (f_src);

  if ((f_result[0] != f_trunc_expected[0])
      || (f_result[1] != f_trunc_expected[1])
      || (f_result[2] != f_trunc_expected[2])
      || (f_result[3] != f_trunc_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_trunc (float) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     f_trunc_expected[0], f_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     f_trunc_expected[1], f_result[1]);
      printf("   expected[2] = %f; result[2] = %f\n",
	     f_trunc_expected[2], f_result[2]);
      printf("   expected[3] = %f; result[3] = %f\n",
	     f_trunc_expected[3], f_result[3]);
    }
#else
  abort();
#endif


  /* ABS, double */
  d_result = vec_abs (d_src);

  if ((d_result[0] != d_abs_expected[0]) || (d_result[1] != d_abs_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_abs (double) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     d_abs_expected[0], d_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     d_abs_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  /* CEIL, double */
  d_result = vec_ceil (d_src);

  if ((d_result[0] != d_ceil_expected[0])
      || (d_result[1] != d_ceil_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_ceil (double) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     d_ceil_expected[0], d_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     d_ceil_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  /* Floor, double */
  d_result = vec_floor (d_src);

  if ((d_result[0] != d_floor_expected[0])
      || (d_result[1] != d_floor_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_floor (double) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     d_floor_expected[0], d_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     d_floor_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  /* Nearby int, double */
  d_result = vec_nearbyint (d_src);

  if ((d_result[0] != d_nearbyint_expected[0])
      || (d_result[1] != d_nearbyint_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_nearbyint (double) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     d_nearbyint_expected[0], d_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     d_nearbyint_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  /* Round int, double */
  d_result = vec_rint (d_src);

  if ((d_result[0] != d_rint_expected[0])
      || (d_result[1] != d_rint_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_rint (double) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     d_rint_expected[0], d_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     d_rint_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  /* Square root, double */
  d_result = vec_sqrt (vec_abs(d_src));

  if ((d_result[0] != d_sqrt_expected[0])
      || (d_result[1] != d_sqrt_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_sqrt (double) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     d_sqrt_expected[0], d_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     d_sqrt_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  /* Truncate, double */
  d_result = vec_trunc (d_src);

  if ((d_result[0] != d_trunc_expected[0])
      || (d_result[1] != d_trunc_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_trunc (double) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     d_trunc_expected[0], d_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     d_trunc_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  return 0;
}  
