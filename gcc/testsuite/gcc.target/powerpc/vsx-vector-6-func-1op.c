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
#define FLOAT_CHECK(NAME)                                                  \
  f_result = vec_##NAME(f_src);                                            \
                                                                           \
  if ((f_result[0] != f_##NAME##_expected[0]) ||                           \
      (f_result[1] != f_##NAME##_expected[1]) ||                           \
      (f_result[2] != f_##NAME##_expected[2]) ||                           \
      (f_result[3] != f_##NAME##_expected[3]))                             \
    {                                                                      \
      if (DEBUG) {                                                         \
        printf("ERROR: vec_%s (float) expected value does not match\n",    \
               #NAME);							   \
        printf("   expected[0] = %f; result[0] = %f\n",                    \
	       f_##NAME##_expected[0], f_result[0]);                       \
        printf("   expected[1] = %f; result[1] = %f\n",                    \
	       f_##NAME##_expected[1], f_result[1]);                       \
        printf("   expected[2] = %f; result[2] = %f\n",                    \
	       f_##NAME##_expected[2], f_result[2]);                       \
        printf("   expected[3] = %f; result[3] = %f\n",                    \
	       f_##NAME##_expected[3], f_result[3]);                       \
      } else                                                               \
        abort();                                                           \
  }

  /* Macro to check the results for the various double point argument tests.
   */
#define DOUBLE_CHECK(NAME)                                                 \
  d_result = vec_##NAME(d_src);                                            \
                                                                           \
  if ((d_result[0] != d_##NAME##_expected[0])				   \
      || (d_result[1] != d_##NAME##_expected[1]))                          \
    {									   \
      if (DEBUG)							   \
        {                                                                  \
          printf("ERROR: vec_%s (double) expected value does not match\n", \
	         #NAME);                                                   \
          printf("   expected[0] = %f; result[0] = %f\n",                  \
	         d_##NAME##_expected[0], d_result[0]);                     \
          printf("   expected[1] = %f; result[1] = %f\n",                  \
   	         d_##NAME##_expected[1], d_result[1]);                     \
         }                                                                 \
      else                                                                 \
        abort();                                                           \
  }

void
float_tests (vector float f_src, vector float f_abs_expected,
	     vector float f_ceil_expected, vector float f_floor_expected,
	     vector float f_nearbyint_expected, vector float f_rint_expected,
	     vector float f_sqrt_expected, vector float f_trunc_expected)
{
  /* Some of the tests are simple enough that the compiler computes the answer
     at compile time.  Put the tests in a function so the compiler will generate
     the code needed to perform the test.  */
  vector float f_result;

  FLOAT_CHECK (abs)
  FLOAT_CHECK (ceil)
  FLOAT_CHECK (floor)
  FLOAT_CHECK (nearbyint)
  FLOAT_CHECK (rint)
  FLOAT_CHECK (abs)
  FLOAT_CHECK (trunc)
}

void
double_tests (vector double d_src, vector double d_abs_expected,
	      vector double d_ceil_expected, vector double d_floor_expected,
	      vector double d_nearbyint_expected, vector double d_rint_expected,
	      vector double d_sqrt_expected, vector double d_trunc_expected)
{
  /* Some of the tests are simple enough that the compiler computes the answer
     at compile time.  Put the tests in a function so the compiler will generate
     the code needed to perform the test.  */
  vector double d_result;

  DOUBLE_CHECK (abs)
  DOUBLE_CHECK (ceil)
  DOUBLE_CHECK (floor)
  DOUBLE_CHECK (nearbyint)
  DOUBLE_CHECK (rint)
  DOUBLE_CHECK (abs)
  DOUBLE_CHECK (trunc)

  /* Need to make sure the arguments for sqrt are always positive.  Do this
     test last as we have to change the input for the test.  */
  d_src = vec_abs (d_src);
  DOUBLE_CHECK (sqrt)
}
		  
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
  float_tests (f_src, f_abs_expected, f_ceil_expected, f_floor_expected,
	       f_nearbyint_expected, f_rint_expected, f_sqrt_expected,
	       f_trunc_expected);

  /* Double argument tests. */
  double_tests (d_src, d_abs_expected, d_ceil_expected, d_floor_expected,
		d_nearbyint_expected, d_rint_expected, d_sqrt_expected,
		d_trunc_expected);

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
