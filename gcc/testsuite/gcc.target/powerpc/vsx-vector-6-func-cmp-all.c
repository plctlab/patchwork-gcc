/* { dg-do compile { target lp64 } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } } */
/* { dg-options "-O2 -mdejagnu-cpu=power7" } */

/* Functional test of the vector compare all builtins.  */

#include <altivec.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define DEBUG 0

void abort (void);

int
main () {
  int i;
  vector float f_src_a = {126.0, 23.0, -338.0, 17.0};
  vector float f_src_b = {2.00, 23.0, 1.0, 4.0};
  bool f_result;
  bool f_all_eq_expected = 0;
  bool f_all_gt_expected = 0;
  bool f_all_ge_expected = 0;
  bool f_all_lt_expected = 0;
  bool f_all_le_expected = 0;
  bool f_all_nan_expected = 0;
  bool f_all_numeric_expected = 1;
  bool f_any_eq_expected = 1;
  bool f_any_gt_expected = 1;
  bool f_any_ge_expected = 1;
  bool f_any_lt_expected = 1;
  bool f_any_le_expected = 1;
  bool f_any_nan_expected = 0;
  bool f_any_numeric_expected = 1;

  vector double d_src_a = { 125.44, -338.56};
  vector double d_src_b = d_src_a;
  bool d_result;
  bool d_all_eq_expected = 1;
  bool d_all_gt_expected = 0;
  bool d_all_ge_expected = 1;
  bool d_all_lt_expected = 0;
  bool d_all_le_expected = 1;
  bool d_all_nan_expected = 0;
  bool d_all_numeric_expected = 1;
  bool d_any_eq_expected = 1;
  bool d_any_gt_expected = 0;
  bool d_any_ge_expected = 1;
  bool d_any_lt_expected = 0;
  bool d_any_le_expected = 1;
  bool d_any_nan_expected = 0;
  bool d_any_numeric_expected = 1;

  /* Compare all equal, float */
  f_result = vec_all_eq (f_src_a, f_src_b);

  if (f_result != f_all_eq_expected)
#if DEBUG
    {
      printf("ERROR: vec_all_eq (float) expected value does not match\n");
      printf("   expected = %d; result = %d\n",
	     f_all_eq_expected, f_result);
    }
#else
  abort();
#endif

  /* Compare all greater than, float */
  f_result = vec_all_gt (f_src_a, f_src_b);

  if (f_result != f_all_gt_expected)
#if DEBUG
    {
      printf("ERROR: vec_all_gt (float) expected value does not match\n");
      printf("   expected = %d; result = %d\n",
	     f_all_gt_expected, f_result);
    }
#else
  abort();
#endif

  /* Compare all greater than or equal, float */
  f_result = vec_all_ge (f_src_a, f_src_b);

  if (f_result != f_all_ge_expected)
#if DEBUG
    {
      printf("ERROR: vec_all_ge(float) expected value does not match\n");
      printf("   expected] = %d; result = %d\n", f_all_ge_expected, f_result);
    }
#else
  abort();
#endif

  /* Compare all less than, float */
  f_result = vec_all_lt (f_src_a, f_src_b);

  if (f_result != f_all_lt_expected)
#if DEBUG
    {
      printf("ERROR: vec_all_lt (float) expected value does not match\n");
      printf("   expected = %d; result = %d\n", f_all_lt_expected, f_result);
    }
#else
  abort();
#endif

  /* Compare all less than or equal, float */
  f_result = vec_all_le (f_src_a, f_src_b);

  if (f_result != f_all_le_expected)
#if DEBUG
    {
      printf("ERROR: vec_all_le (float) expected value does not match\n");
      printf("   expected = %d; result = %d\n", f_all_le_expected, f_result);
    }
#else
  abort();
#endif

  /* All NaN, float */
  f_result = vec_all_nan (f_src_a);

  if (f_result != f_all_nan_expected)
#if DEBUG
    {
      printf("ERROR: vec_all_le (float) expected value does not match\n");
      printf("   expected = %d; result = %d\n", f_all_nan_expected, f_result);
    }
#else
  abort();
#endif

  /* All numeric, float */
  f_result = vec_all_numeric (f_src_a);

  if (f_result != f_all_numeric_expected)
#if DEBUG
    {
      printf("ERROR: vec_all_numeric (float) expected value does not match\n");
      printf("   expected = %d; result = %d\n", f_all_numeric_expected,
	     f_result);
    }
#else
  abort();
#endif


  /* Compare any equal, float */
  f_result = vec_any_eq (f_src_a, f_src_b);

  if (f_result != f_any_eq_expected)
#if DEBUG
    {
      printf("ERROR: vec_any_eq (float) expected value does not match\n");
      printf("   expected = %d; result = %d\n",
	     f_any_eq_expected, f_result);
    }
#else
  abort();
#endif

  /* Compare any greater than, float */
  f_result = vec_any_gt (f_src_a, f_src_b);

  if (f_result != f_any_gt_expected)
#if DEBUG
    {
      printf("ERROR: vec_any_gt (float) expected value does not match\n");
      printf("   expected = %d; result = %d\n",
	     f_any_gt_expected, f_result);
    }
#else
  abort();
#endif

  /* Compare any greater than or equal, float */
  f_result = vec_any_ge (f_src_a, f_src_b);

  if (f_result != f_any_ge_expected)
#if DEBUG
    {
      printf("ERROR: vec_any_ge(float) expected value does not match\n");
      printf("   expected] = %d; result = %d\n", f_any_ge_expected, f_result);
    }
#else
  abort();
#endif

  /* Compare any less than, float */
  f_result = vec_any_lt (f_src_a, f_src_b);

  if (f_result != f_any_lt_expected)
#if DEBUG
    {
      printf("ERROR: vec_any_lt (float) expected value does not match\n");
      printf("   expected = %d; result = %d\n", f_any_lt_expected, f_result);
    }
#else
  abort();
#endif

  /* Compare any less than or equal, float */
  f_result = vec_any_le (f_src_a, f_src_b);

  if (f_result != f_any_le_expected)
#if DEBUG
    {
      printf("ERROR: vec_any_le (float) expected value does not match\n");
      printf("   expected = %d; result = %d\n", f_any_le_expected, f_result);
    }
#else
  abort();
#endif

  /* Any NaN, float */
  f_result = vec_any_nan (f_src_a);

  if (f_result != f_any_nan_expected)
#if DEBUG
    {
      printf("ERROR: vec_any_le (float) expected value does not match\n");
      printf("   expected = %d; result = %d\n", f_any_nan_expected, f_result);
    }
#else
  abort();
#endif

  /* Any numeric, float */
  f_result = vec_any_numeric (f_src_a);

  if (f_result != f_any_numeric_expected)
#if DEBUG
    {
      printf("ERROR: vec_any_numeric (float) expected value does not match\n");
      printf("   expected = %d; result = %d\n", f_any_numeric_expected,
	     f_result);
    }
#else
  abort();
#endif

  /* Compare all equal, double */
  d_result = vec_all_eq (d_src_a, d_src_b);

  if (d_result != d_all_eq_expected)
#if DEBUG
    {
      printf("ERROR: vec_all_eq (double) expected value does not match\n");
      printf("   expected] = %d; result = %d\n", d_all_eq_expected, d_result);
    }
#else
  abort();
#endif

  /* Compare all greater than, double */
  d_result = vec_all_gt (d_src_a, d_src_b);

  if (d_result != d_all_gt_expected)
#if DEBUG
    {
      printf("ERROR: vec_all_gt (double) expected value does not match\n");
      printf("   expected = %d; result = %d\n", d_all_gt_expected, d_result);
    }
#else
  abort();
#endif

  /* Compare all greater than or equal, double */
  d_result = vec_all_ge (d_src_a, d_src_b);

  if (d_result != d_all_ge_expected)
#if DEBUG
    {
      printf("ERROR: vec_all_ge (double) expected value does not match\n");
      printf("   expected] = %d; result = %d\n", d_all_gt_expected, d_result);
    }
#else
  abort();
#endif

  /* Compare all less than, double */
  d_result = vec_all_lt (d_src_a, d_src_b);

  if (d_result != d_all_lt_expected)
#if DEBUG
    {
      printf("ERROR: vec_all_lt (double) expected value does not match\n");
      printf("   expected = %d; result = %d\n", d_all_lt_expected, d_result);
    }
#else
  abort();
#endif

  /* Compare all less than or equal, double */
  d_result = vec_all_le (d_src_a, d_src_b);

  if (d_result != d_all_le_expected)
#if DEBUG
    {
      printf("ERROR: vec_all_le (double) expected value does not match\n");
      printf("   expected = %d; result = %d\n", d_all_le_expected, d_result);
    }
#else
  abort();
#endif

    /* All NaN, double */
  d_result = vec_all_nan (d_src_a);

  if (d_result != d_all_nan_expected)
#if DEBUG
    {
      printf("ERROR: vec_all_nan (double) expected value does not match\n");
      printf("   expected = %d; result = %d\n", d_all_nan_expected, d_result);
    }
#else
  abort();
#endif

  /* All numeric double */
  d_result = vec_all_numeric (d_src_a);

  if (d_result != d_all_numeric_expected)
#if DEBUG
    {
      printf("ERROR: vec_all_numeric (double) expected value does not match\n");
      printf("   expected = %d; result = %d\n", d_all_numeric_expected,
	     d_result);
    }
#else
  abort();
#endif

  /* Compare any equal, double */
  d_result = vec_any_eq (d_src_a, d_src_b);

  if (d_result != d_any_eq_expected)
#if DEBUG
    {
      printf("ERROR: vec_any_eq (double) expected value does not match\n");
      printf("   expected] = %d; result = %d\n", d_any_eq_expected, d_result);
    }
#else
  abort();
#endif

  /* Compare any greater than, double */
  d_result = vec_any_gt (d_src_a, d_src_b);

  if (d_result != d_any_gt_expected)
#if DEBUG
    {
      printf("ERROR: vec_any_gt (double) expected value does not match\n");
      printf("   expected = %d; result = %d\n", d_any_gt_expected, d_result);
    }
#else
  abort();
#endif

  /* Compare any greater than or equal, double */
  d_result = vec_any_ge (d_src_a, d_src_b);

  if (d_result != d_any_ge_expected)
#if DEBUG
    {
      printf("ERROR: vec_any_ge (double) expected value does not match\n");
      printf("   expected] = %d; result = %d\n", d_any_gt_expected, d_result);
    }
#else
  abort();
#endif

  /* Compare any less than, double */
  d_result = vec_any_lt (d_src_a, d_src_b);

  if (d_result != d_any_lt_expected)
#if DEBUG
    {
      printf("ERROR: vec_any_lt (double) expected value does not match\n");
      printf("   expected = %d; result = %d\n", d_any_lt_expected, d_result);
    }
#else
  abort();
#endif

  /* Compare any less than or equal, double */
  d_result = vec_any_le (d_src_a, d_src_b);

  if (d_result != d_any_le_expected)
#if DEBUG
    {
      printf("ERROR: vec_any_le (double) expected value does not match\n");
      printf("   expected = %d; result = %d\n", d_any_le_expected, d_result);
    }
#else
  abort();
#endif

    /* Any NaN, float */
  d_result = vec_any_nan (d_src_a);

  if (d_result != d_any_nan_expected)
#if DEBUG
    {
      printf("ERROR: vec_any_nan (double) expected value does not match\n");
      printf("   expected = %d; result = %d\n", d_any_nan_expected, d_result);
    }
#else
  abort();
#endif

  /* Any numeric, double */
  d_result = vec_any_numeric (d_src_a);

  if (d_result != d_any_numeric_expected)
#if DEBUG
    {
      printf("ERROR: vec_any_numeric (double) expected value does not match\n");
      printf("   expected = %d; result = %d\n", d_any_numeric_expected,
	     d_result);
    }
#else
  abort();
#endif

  return 0;
}
