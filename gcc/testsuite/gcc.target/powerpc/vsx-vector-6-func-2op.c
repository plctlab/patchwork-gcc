/* { dg-do run { target lp64 } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } } */
/* { dg-options "-O2 -save-temps" } */
/* Functional test of the two operand vector builtins.  */

#include <altivec.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0

#define FLOAT_TEST(NAME)						    \
  void __attribute__ ((noipa))                                              \
  float_##NAME(vector float f_src_a, vector float f_src_b,		    \
	       vector float f_##NAME##_expected)			    \
  {									    \
    vector float f_result = vec_##NAME (f_src_a, f_src_b);		    \
                                                                            \
    if ((f_result[0] != f_##NAME##_expected[0])				    \
	|| (f_result[1] != f_##NAME##_expected[1])			    \
	|| (f_result[2] != f_##NAME##_expected[2])			    \
	|| (f_result[3] != f_##NAME##_expected[3]))			    \
      {									    \
	if (DEBUG)							    \
	  {								    \
	    printf("ERROR: vec_%s (float) expected value does not match\n", \
		   #NAME);						    \
	    printf("   expected[0] = %f; result[0] = %f\n",		    \
		   f_##NAME##_expected[0], f_result[0]);		    \
	    printf("   expected[1] = %f; result[1] = %f\n",		    \
		   f_##NAME##_expected[1], f_result[1]);		    \
	    printf("   expected[2] = %f; result[2] = %f\n",		    \
		   f_##NAME##_expected[2], f_result[2]);		    \
	    printf("   expected[3] = %f; result[3] = %f\n",		    \
		   f_##NAME##_expected[3], f_result[3]);		    \
	  }								    \
	else								    \
	  abort();							    \
      }									    \
  }

FLOAT_TEST (add)
FLOAT_TEST (div)
FLOAT_TEST (max)
FLOAT_TEST (min)
FLOAT_TEST (mul)
FLOAT_TEST (sub)

#define DOUBLE_TEST(NAME)						    \
  void __attribute__ ((noipa))                                              \
  double_##NAME(vector double d_src_a, vector double d_src_b,		    \
                vector double d_##NAME##_expected)                          \
  {									    \
    vector double d_result = vec_##NAME (d_src_a, d_src_b);		    \
                                                                            \
    if ((d_result[0] != d_##NAME##_expected[0])				    \
	|| (d_result[1] != d_##NAME##_expected[1]))			    \
      {									    \
	if (DEBUG)							    \
	  {								    \
	    printf("ERROR: vec_%s(double) expected value does not match\n", \
		   #NAME);						    \
	    printf("   expected[0] = %f; result[0] = %f\n",		    \
		   d_##NAME##_expected[0], d_result[0]);		    \
	    printf("   expected[1] = %f; result[1] = %f\n",		    \
		   d_##NAME##_expected[1], d_result[1]);		    \
	  }								    \
	else								    \
	  abort();							    \
      }									    \
  }

DOUBLE_TEST (add)
DOUBLE_TEST (div)
DOUBLE_TEST (max)
DOUBLE_TEST (min)
DOUBLE_TEST (mul)
DOUBLE_TEST (sub)

void abort (void);

int
main () {
  int i;
  vector float f_src_a = { 126.0, 23.0, -338.0, 17.0};
  vector float f_src_b = { 2.00, -4.0, 1.0, 4.0};
  vector float f_add_expected = { 128.0, 19.0, -337.0, 21.0};
  vector float f_div_expected = { 63.0, -5.75, -338, 4.25};
  vector float f_max_expected = { 126.0, 23.0, 1.0, 17.0};
  vector float f_min_expected = { 2.0, -4.0, -338.0, 4.0};
  vector float f_mul_expected = { 252, -92.0, -338, 68.0};
  vector float f_sub_expected = { 124.0, 27.0, -339.0, 13.0};

  vector double d_src_a = { 125.44, -338.56};
  vector double d_src_b = { 4.0, -2.0};
  vector double d_add_expected = { 129.44, -340.56};
  vector double d_div_expected = { 31.360000, 169.280000};
  vector double d_max_expected = { 125.44, -2.0};
  vector double d_min_expected = { 4.0, -338.56};
  vector double d_mul_expected = { 501.760000, 677.120000};
  vector double d_sub_expected = { 121.440000, -336.560000};

  /* Two argument float tests.  */
  float_add (f_src_a, f_src_b, f_add_expected);
  float_div (f_src_a, f_src_b, f_div_expected);
  float_max (f_src_a, f_src_b, f_max_expected);
  float_min (f_src_a, f_src_b, f_min_expected);
  float_mul (f_src_a, f_src_b, f_mul_expected);
  float_sub (f_src_a, f_src_b, f_sub_expected);
  
  /* Two argument double tests.  */
  double_add (d_src_a, d_src_b, d_add_expected);
  double_div (d_src_a, d_src_b, d_div_expected);
  double_max (d_src_a, d_src_b, d_max_expected);
  double_min (d_src_a, d_src_b, d_min_expected);
  double_mul (d_src_a, d_src_b, d_mul_expected);
  double_sub (d_src_a, d_src_b, d_sub_expected);

  return 0;
}

/* { dg-final { scan-assembler-times {\mxvaddsp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvdivsp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvmulsp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvsubsp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvadddp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvdivdp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvmindp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvmuldp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvsubdp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvmaxsp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvminsp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvmaxdp\M} 1 } } */
