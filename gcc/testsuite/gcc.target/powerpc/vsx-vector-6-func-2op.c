/* { dg-do compile { target lp64 } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } } */
/* { dg-options "-O2 -mdejagnu-cpu=power7" } */
/* Functional test of the two operand vector builtins.  */

#include <altivec.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0

void abort (void);

int
main () {
  int i;
  vector float f_src_a = { 126.0, 23.0, -338.0, 17.0};
  vector float f_src_b = { 2.00, -4.0, 1.0, 4.0};
  vector float f_result;
  vector float f_add_expected = { 128.0, 19.0, -337.0, 21.0};
  vector float f_div_expected = { 63.0, -5.75, -338, 4.25};
  vector float f_max_expected = { 126.0, 23.0, 1.0, 17.0};
  vector float f_min_expected = { 2.0, -4.0, -338.0, 4.0};
  vector float f_mul_expected = { 252, -92.0, -338, 68.0};
  vector float f_sub_expected = { 124.0, 27.0, -339.0, 13.0};

  vector double d_src_a = { 125.44, -338.56};
  vector double d_src_b = { 4.0, -2.0};
  vector double d_result;
  vector double d_add_expected = { 129.44, -340.56};
  vector double d_div_expected = { 31.360000, 169.280000};
  vector double d_max_expected = { 125.44, -2.0};
  vector double d_min_expected = { 4.0, -338.56};
  vector double d_mul_expected = { 501.760000, 677.120000};
  vector double d_sub_expected = { 121.440000, -336.560000};

  /* Add, float */
  f_result = vec_add (f_src_a, f_src_b);

  if ((f_result[0] != f_add_expected[0])
      || (f_result[1] != f_add_expected[1])
      || (f_result[2] != f_add_expected[2])
      || (f_result[3] != f_add_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_add (float) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     f_add_expected[0], f_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     f_add_expected[1], f_result[1]);
      printf("   expected[2] = %f; result[2] = %f\n",
	     f_add_expected[2], f_result[2]);
      printf("   expected[3] = %f; result[3] = %f\n",
	     f_add_expected[3], f_result[3]);
    }
#else
  abort();
#endif

  /* Divide, float */
  f_result = vec_div (f_src_a, f_src_b);

  if ((f_result[0] != f_div_expected[0])
      || (f_result[1] != f_div_expected[1])
      || (f_result[2] != f_div_expected[2])
      || (f_result[3] != f_div_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_div (float) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     f_div_expected[0], f_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     f_div_expected[1], f_result[1]);
      printf("   expected[2] = %f; result[2] = %f\n",
	     f_div_expected[2], f_result[2]);
      printf("   expected[3] = %f; result[3] = %f\n",
	     f_div_expected[3], f_result[3]);
    }
#else
  abort();
#endif

  /* Floor, max */
  f_result = vec_max (f_src_a, f_src_b);

  if ((f_result[0] != f_max_expected[0])
      || (f_result[1] != f_max_expected[1])
      || (f_result[2] != f_max_expected[2])
      || (f_result[3] != f_max_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_max(float) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     f_max_expected[0], f_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     f_max_expected[1], f_result[1]);
      printf("   expected[2] = %f; result[2] = %f\n",
	     f_max_expected[2], f_result[2]);
      printf("   expected[3] = %f; result[3] = %f\n",
	     f_max_expected[3], f_result[3]);
    }
#else
  abort();
#endif

  /* Minimum, float */
  f_result = vec_min (f_src_a, f_src_b);

  if ((f_result[0] != f_min_expected[0])
      || (f_result[1] != f_min_expected[1])
      || (f_result[2] != f_min_expected[2])
      || (f_result[3] != f_min_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_min (float) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     f_min_expected[0], f_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     f_min_expected[1], f_result[1]);
      printf("   expected[2] = %f; result[2] = %f\n",
	     f_min_expected[2], f_result[2]);
      printf("   expected[3] = %f; result[3] = %f\n",
	     f_min_expected[3], f_result[3]);
    }
#else
  abort();
#endif

  /* Multiply, float */
  f_result = vec_mul (f_src_a, f_src_b);

  if ((f_result[0] != f_mul_expected[0])
      || (f_result[1] != f_mul_expected[1])
      || (f_result[2] != f_mul_expected[2])
      || (f_result[3] != f_mul_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_mult (float) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     f_mul_expected[0], f_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     f_mul_expected[1], f_result[1]);
      printf("   expected[2] = %f; result[2] = %f\n",
	     f_mul_expected[2], f_result[2]);
      printf("   expected[3] = %f; result[3] = %f\n",
	     f_mul_expected[3], f_result[3]);
    }
#else
  abort();
#endif

    /* Subtract, float */
  f_result = vec_sub (f_src_a, f_src_b);

  if ((f_result[0] != f_sub_expected[0])
      || (f_result[1] != f_sub_expected[1])
      || (f_result[2] != f_sub_expected[2])
      || (f_result[3] != f_sub_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_sub (float) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     f_sub_expected[0], f_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     f_sub_expected[1], f_result[1]);
      printf("   expected[2] = %f; result[2] = %f\n",
	     f_sub_expected[2], f_result[2]);
      printf("   expected[3] = %f; result[3] = %f\n",
	     f_sub_expected[3], f_result[3]);
    }
#else
  abort();
#endif



  /* Add, double */
  d_result = vec_add (d_src_a, d_src_b);

  if ((d_result[0] != d_add_expected[0]) || (d_result[1] != d_add_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_add (double) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     d_add_expected[0], d_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     d_add_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  /* Divide, double */
  d_result = vec_div (d_src_a, d_src_b);

  if ((d_result[0] != d_div_expected[0])
      || (d_result[1] != d_div_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_div (double) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     d_div_expected[0], d_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     d_div_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  /* Max */
  d_result = vec_max (d_src_a, d_src_b);

  if ((d_result[0] != d_max_expected[0])
      || (d_result[1] != d_max_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_max (double) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     d_max_expected[0], d_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     d_max_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  /* Nearby min */
  d_result = vec_min (d_src_a, d_src_b);

  if ((d_result[0] != d_min_expected[0]) || (d_result[1] != d_min_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_min (double) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     d_min_expected[0], d_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     d_min_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  /* Multiply, double */
  d_result = vec_mul (d_src_a, d_src_b);

  if ((d_result[0] != d_mul_expected[0])
      || (d_result[1] != d_mul_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_mul (double) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     d_mul_expected[0], d_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     d_mul_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  /* Subract root, double */
  d_result = vec_sub (d_src_a, d_src_b);

  if ((d_result[0] != d_sub_expected[0])
      || (d_result[1] != d_sub_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_sub (double) expected value does not match\n");
      printf("   expected[0] = %f; result[0] = %f\n",
	     d_sub_expected[0], d_result[0]);
      printf("   expected[1] = %f; result[1] = %f\n",
	     d_sub_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  return 0;
}
