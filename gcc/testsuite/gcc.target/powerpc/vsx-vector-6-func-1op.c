/* { dg-do run { target lp64 } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } } */
/* { dg-options "-O2 -save-temps" } */

/* Functional test of the one operand vector builtins.  */

#include <altivec.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0

void abort (void);

/* Macro to check the results for the various floating point argument tests.
 */
#define FLOAT_TEST(NAME)                                                  \
  void __attribute__ ((noipa))                                            \
  float_##NAME (vector float f_src, vector float f_##NAME##_expected)     \
  {									  \
    vector float f_result = vec_##NAME(f_src);				  \
                                                                          \
    if ((f_result[0] != f_##NAME##_expected[0]) ||			  \
	(f_result[1] != f_##NAME##_expected[1]) ||			  \
	(f_result[2] != f_##NAME##_expected[2]) ||			  \
	(f_result[3] != f_##NAME##_expected[3]))			  \
      {									  \
	if (DEBUG) {							  \
	  printf("ERROR: vec_%s (float) expected value does not match\n", \
		 #NAME);						  \
	  printf("   expected[0] = %f; result[0] = %f\n",		  \
		 f_##NAME##_expected[0], f_result[0]);			  \
	  printf("   expected[1] = %f; result[1] = %f\n",		  \
		 f_##NAME##_expected[1], f_result[1]);			  \
	  printf("   expected[2] = %f; result[2] = %f\n",		  \
		 f_##NAME##_expected[2], f_result[2]);			  \
	  printf("   expected[3] = %f; result[3] = %f\n",		  \
		 f_##NAME##_expected[3], f_result[3]);			  \
	} else								  \
	  abort();							  \
      }									  \
  }

FLOAT_TEST (abs)
FLOAT_TEST (ceil)
FLOAT_TEST (floor)
FLOAT_TEST (nearbyint)
FLOAT_TEST (rint)
FLOAT_TEST (trunc)

/* Macro to check the results for the various double point argument tests.  */
#define DOUBLE_TEST(NAME)                                                    \
  void __attribute__ ((noipa))                                               \
  double_##NAME (vector double d_src, vector double d_##NAME##_expected)     \
  {									     \
    vector double d_result = vec_##NAME(d_src);				     \
                                                                             \
    if ((d_result[0] != d_##NAME##_expected[0])				     \
	|| (d_result[1] != d_##NAME##_expected[1]))			     \
      {									     \
	if (DEBUG)							     \
	  {								     \
	    printf("ERROR: vec_%s (double) expected value does not match\n", \
		   #NAME);						     \
	    printf("   expected[0] = %f; result[0] = %f\n",		     \
		   d_##NAME##_expected[0], d_result[0]);		     \
	    printf("   expected[1] = %f; result[1] = %f\n",		     \
		   d_##NAME##_expected[1], d_result[1]);		     \
	  }								     \
	else								     \
	  abort();							     \
      }									     \
  }

DOUBLE_TEST (abs)
DOUBLE_TEST (ceil)
DOUBLE_TEST (floor)
DOUBLE_TEST (nearbyint)
DOUBLE_TEST (rint)
DOUBLE_TEST (trunc)
DOUBLE_TEST (sqrt)

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
  vector double d_src_sqrt = { 125.44, 338.56};
  vector double d_abs_src;
  vector double d_result;
  vector double d_abs_expected = { 125.44, 338.56};
  vector double d_ceil_expected = { 126.0, -338.0};
  vector double d_floor_expected = { 125.0, -339.0};
  vector double d_nearbyint_expected = { 125.0, -339.0};
  vector double d_rint_expected = { 125.0, -339.0};
  vector double d_sqrt_expected = { 11.2, 18.4};
  vector double d_trunc_expected = { 125.0, -338.0};


  /* Floating point argument tests. */
  float_abs (f_src, f_abs_expected);
  float_ceil (f_src, f_ceil_expected);
  float_floor (f_src, f_floor_expected);
  float_nearbyint (f_src, f_nearbyint_expected);
  float_rint (f_src, f_rint_expected);
  float_trunc (f_src, f_trunc_expected);

  /* Double argument tests. */
  double_abs (d_src, d_abs_expected);
  double_ceil (d_src, d_ceil_expected);
  double_floor (d_src, d_floor_expected);
  double_nearbyint (d_src, d_nearbyint_expected);
  double_rint (d_src, d_rint_expected);
  double_trunc (d_src, d_trunc_expected);
  double_sqrt (d_src_sqrt, d_sqrt_expected);

  return 0;
}  

/* { dg-final { scan-assembler-times {\mxvabssp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvrspip\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvrspim\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvrspi\M} 1 } } */ 
/* { dg-final { scan-assembler-times {\mxvrspic\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvrspiz\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvabsdp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvrdpip\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvrdpim\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvrdpi\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvrdpic\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvrdpiz\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvsqrtdp\M} 1 } } */
