/* { dg-do run { target lp64 } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } } */
/* { dg-options "-O2 -save-temps" } */

/* Functional test of the vector compare all builtins.  */

#include <altivec.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define DEBUG 0

#define FLOAT_1ARG_TEST(NAME)                                                  \
  void __attribute__ ((noipa))                                                 \
  float_1arg_##NAME (vector float f_src, bool f_##NAME##_expected)             \
  {                                                                            \
    bool f_result = vec_##NAME (f_src); 		                       \
                                                                               \
    if (f_result != f_##NAME##_expected)				       \
      {									       \
	if (DEBUG)							       \
	  {								       \
	    printf("ERROR: vec_%s (float) expected value does not match\n",    \
		   #NAME);						       \
	    printf("   expected = %d; result = %d\n",			       \
		   f_##NAME##_expected, f_result);			       \
	  }								       \
	else								       \
	  abort();							       \
      }									       \
  }

FLOAT_1ARG_TEST (all_nan)
FLOAT_1ARG_TEST (all_numeric)
FLOAT_1ARG_TEST (any_nan)
FLOAT_1ARG_TEST (any_numeric)

#define FLOAT_2ARG_TEST(NAME)						      \
  void __attribute__ ((noipa))                                                \
  float_2arg_##NAME (vector float f_src_a, vector float f_src_b,              \
		     bool f_##NAME##_expected)				      \
  {                                                                           \
    bool f_result = vec_##NAME (f_src_a, f_src_b);			      \
                                                                              \
    if (f_result != f_##NAME##_expected)				      \
      {									      \
	if (DEBUG)							      \
	  {								      \
	    printf("ERROR: vec_%s (float, float) expected value does not match\n", \
		   #NAME);						      \
	    printf("   expected = %d; result = %d\n",			      \
		   f_##NAME##_expected, f_result);			      \
	  }								      \
	else								      \
	  abort();							      \
      }									     \
  }
		 
FLOAT_2ARG_TEST (all_eq)
FLOAT_2ARG_TEST (all_gt)
FLOAT_2ARG_TEST (all_ge)
FLOAT_2ARG_TEST (all_lt)
FLOAT_2ARG_TEST (all_le)
FLOAT_2ARG_TEST (any_eq)
FLOAT_2ARG_TEST (any_gt)
FLOAT_2ARG_TEST (any_ge)
FLOAT_2ARG_TEST (any_lt)
FLOAT_2ARG_TEST (any_le)

#define DOUBLE_1ARG_TEST(NAME )						      \
  void __attribute__ ((noipa))                                                \
  double_1arg_##NAME (vector double d_src, bool d_##NAME##_expected)	      \
  {                                                                           \
    bool d_result = vec_##NAME (d_src);                                       \
                                                                              \
  if (d_result != d_##NAME##_expected)                                        \
    {                                                                         \
      if (DEBUG)                                                              \
	{                                                                     \
	  printf("ERROR: vec_%s (double) expected value does not match\n",    \
		 #NAME);                                                      \
	  printf("   expected = %d; result = %d\n",                           \
		 d_##NAME##_expected, d_result);                              \
	}                                                                     \
      else                                                                    \
	abort();                                                              \
    }									      \
  }

DOUBLE_1ARG_TEST (all_nan)
DOUBLE_1ARG_TEST (all_numeric)
DOUBLE_1ARG_TEST (any_nan)
DOUBLE_1ARG_TEST (any_numeric)

#define DOUBLE_2ARG_TEST(NAME)						      \
  void __attribute__ ((noipa))                                                \
  double_2arg_##NAME (vector double d_src_a, vector double d_src_b,           \
		      bool d_##NAME##_expected)	                              \
  {                                                                           \
    bool d_result = vec_##NAME (d_src_a, d_src_b);	                      \
                                                                              \
    if (d_result != d_##NAME##_expected)				      \
      {									      \
	if (DEBUG)							      \
	  {								      \
	    printf("ERROR: vec_%s (double, double) expected value does not match\n", \
		   #NAME);						      \
	    printf("   expected = %d; result = %d\n",			      \
		   d_##NAME##_expected, d_result);			      \
	  }								      \
	else								      \
	  abort();							      \
	}                                                                     \
    }

DOUBLE_2ARG_TEST (all_eq)
DOUBLE_2ARG_TEST (all_gt)
DOUBLE_2ARG_TEST (all_ge)
DOUBLE_2ARG_TEST (all_lt)
DOUBLE_2ARG_TEST (all_le)
DOUBLE_2ARG_TEST (any_eq)
DOUBLE_2ARG_TEST (any_gt)
DOUBLE_2ARG_TEST (any_ge)
DOUBLE_2ARG_TEST (any_lt)
DOUBLE_2ARG_TEST (any_le)

void abort (void);

int
main () {
  int i;
  vector float f_src_a = {126.0, 23.0, -338.0, 17.0};
  vector float f_src_b = {2.00, 23.0, 1.0, 4.0};
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

  /* Run tests.  */
  float_1arg_all_nan (f_src_a, f_all_nan_expected);
  float_1arg_all_numeric (f_src_a, f_all_numeric_expected);
  float_1arg_any_nan (f_src_a, f_any_nan_expected);
  float_1arg_any_numeric (f_src_a, f_any_numeric_expected);

  float_2arg_all_eq (f_src_a, f_src_b, f_all_eq_expected);
  float_2arg_all_gt (f_src_a, f_src_b, f_all_gt_expected);
  float_2arg_all_ge (f_src_a, f_src_b, f_all_ge_expected);
  float_2arg_all_lt (f_src_a, f_src_b, f_all_lt_expected);
  float_2arg_all_le (f_src_a, f_src_b, f_all_le_expected);
  float_2arg_any_eq (f_src_a, f_src_b, f_any_eq_expected);
  float_2arg_any_gt (f_src_a, f_src_b, f_any_gt_expected);
  float_2arg_any_ge (f_src_a, f_src_b, f_any_ge_expected);
  float_2arg_any_lt (f_src_a, f_src_b, f_any_lt_expected);
  float_2arg_any_le (f_src_a, f_src_b, f_any_le_expected);

  double_1arg_all_nan (d_src_a, d_all_nan_expected);
  double_1arg_all_numeric (d_src_a, d_all_numeric_expected);
  double_1arg_any_nan (d_src_a, d_any_nan_expected);
  double_1arg_any_numeric (d_src_a, d_any_numeric_expected);

  double_2arg_all_eq (d_src_a, d_src_b, d_all_eq_expected);
  double_2arg_all_gt (d_src_a, d_src_b, d_all_gt_expected);
  double_2arg_all_ge (d_src_a, d_src_b, d_all_ge_expected);
  double_2arg_all_lt (d_src_a, d_src_b, d_all_lt_expected);
  double_2arg_all_le (d_src_a, d_src_b, d_all_le_expected);
  double_2arg_any_eq (d_src_a, d_src_b, d_any_eq_expected);
  double_2arg_any_gt (d_src_a, d_src_b, d_any_gt_expected);
  double_2arg_any_ge (d_src_a, d_src_b, d_any_ge_expected);
  double_2arg_any_lt (d_src_a, d_src_b, d_any_lt_expected);
  double_2arg_any_le (d_src_a, d_src_b, d_any_le_expected);

  return 0;
}

/* { dg-final { scan-assembler-times {\mxvcmpeqsp\M} 6 } } */
/* { dg-final { scan-assembler-times {\mxvcmpgtsp\M} 4 } } */
/* { dg-final { scan-assembler-times {\mxvcmpgesp\M} 4 } } */
/* { dg-final { scan-assembler-times {\mxvcmpeqdp\M} 6 } } */
/* { dg-final { scan-assembler-times {\mxvcmpgedp\M} 4 } } */
/* { dg-final { scan-assembler-times {\mxvcmpgtdp\M} 4 } } */


