/* { dg-do run { target lp64 } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } } */
/* { dg-options "-O2 -save-temps" } */

/* Functional test of the two operand logical vector builtins.  */

#include <altivec.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0

union conv_t {
  vector float f;
  vector unsigned int u;
  vector double d;
  vector unsigned long ul;
} conv_result, conv_exp, conv_src_a, conv_src_b;

void abort (void);

#define FLOAT_TEST(NAME)                                                    \
  void __attribute__ ((noipa))                                              \
  float_##NAME (vector float f_src_a, vector float f_src_b,                 \
		vector float f_##NAME##_expected)                           \
  {                                                                         \
    int i;                                                                  \
    vector float f_result = vec_##NAME (f_src_a, f_src_b);                  \
                                                                            \
    if ((f_result[0] != f_##NAME##_expected[0])                             \
        || (f_result[1] != f_##NAME##_expected[1])                          \
        || (f_result[2] != f_##NAME##_expected[2])                          \
        || (f_result[3] != f_##NAME##_expected[3]))                         \
      {                                                                     \
        if (DEBUG)                                                          \
	  {                                                                 \
	    printf("ERROR: vec_%s (float) expected value does not match\n", \
                   #NAME);                                                  \
	    for (i = 0; i < 4; i++)                                         \
	      {                                                             \
	        conv_result.f[i] = f_result[i];                             \
	        printf("   expected[%d] = 0x%x; result[%d] = 0x%x\n", i,    \
		       conv_exp.u[i], i, conv_result.u[i]);                 \
	      }                                                             \
	  }                                                                 \
        else                                                                \
	  abort();                                                          \
      }									    \
  }

FLOAT_TEST (and)
FLOAT_TEST (andc)
FLOAT_TEST (nor)
FLOAT_TEST (or)
FLOAT_TEST (xor)

#define DOUBLE_TEST(NAME)						     \
  void __attribute__ ((noipa))                                               \
  double_##NAME (vector double d_src_a, vector double d_src_b,               \
                 vector double d_##NAME##_expected)  			     \
  {                                                                          \
    vector double d_result = vec_##NAME (d_src_a, d_src_b);                  \
    int i;                                                                   \
                                                                             \
    if ((d_result[0] != d_##NAME##_expected[0])			             \
        || (d_result[1] != d_##NAME##_expected[1]))                          \
      {                                                                      \
        if (DEBUG)                                                           \
	  {                                                                  \
	    printf("ERROR: vec_%s (double) expected value does not match\n", \
		   #NAME);                                                   \
	    for (i = 0; i < 2; i++)                                          \
	      {                                                              \
	        conv_result.d[i] = d_result[i];                              \
	        printf("   expected[%d] = 0x%lx; result[%d] = 0x%lx\n", i,   \
		       conv_exp.ul, i, conv_result.ul);                      \
	      }                                                              \
	  }                                                                  \
        else  								     \
	  abort();							     \
      }									     \
  }

DOUBLE_TEST (and)
DOUBLE_TEST (andc)
DOUBLE_TEST (nor)
DOUBLE_TEST (or)
DOUBLE_TEST (xor)

int
main () {
  int i;

  vector float f_src_a = { 1.0, 2.0, 3.0, 4.0};
  vector float f_src_b = { 1.0, 3.0, -3.0, 2.0};
  vector float f_and_expected, f_andc_expected, f_nor_expected, f_or_expected;
  vector float f_xor_expected;

  vector double d_src_a = { 8.0, 10.0};
  vector double d_src_b = { 12.0, 2.0};
  vector double d_result;
  vector double d_and_expected, d_andc_expected, d_nor_expected;
  vector double d_or_expected, d_xor_expected;

  /* Calculate expected results.  */
  /* AND, float */
  for (i = 0; i < 4; i++)
    {
      conv_src_a.f[i] = f_src_a[i];
      conv_src_b.f[i] = f_src_b[i];
      conv_exp.u[i] = conv_src_a.u[i] & conv_src_b.u[i];
      f_and_expected[i] = conv_exp.f[i];
    }

  /* ANDC, float */
  for (i = 0; i < 4; i++)
    {
      conv_src_a.f[i] = f_src_a[i];
      conv_src_b.f[i] = f_src_b[i];
      conv_exp.u[i] = conv_src_a.u[i] & ~conv_src_b.u[i];
      f_andc_expected[i] = conv_exp.f[i];
    }

    /* NOR, max */
  for (i = 0; i < 4; i++)
    {
      conv_src_a.f[i] = f_src_a[i];
      conv_src_b.f[i] = f_src_b[i];
      conv_exp.u[i] = ~(conv_src_a.u[i] | conv_src_b.u[i]);
      f_nor_expected[i] = conv_exp.f[i];
    }

  /* OR, float */
  for (i = 0; i < 4; i++)
    {
      conv_src_a.f[i] = f_src_a[i];
      conv_src_b.f[i] = f_src_b[i];
      conv_exp.u[i] = conv_src_a.u[i] | conv_src_b.u[i];
      f_or_expected[i] = conv_exp.f[i];
    }

  /* XOR, float */
  for (i = 0; i < 4; i++)
    {
      conv_src_a.f[i] = f_src_a[i];
      conv_src_b.f[i] = f_src_b[i];
      conv_exp.u[i] = conv_src_a.u[i] ^ conv_src_b.u[i];
      f_xor_expected[i] = conv_exp.f[i];
    }

  /* AND, double */
  for (i = 0; i < 2; i++)
    {
      conv_src_a.d[i] = d_src_a[i];
      conv_src_b.d[i] = d_src_b[i];
      conv_exp.ul[i] = conv_src_a.ul[i] & conv_src_b.ul[i];
      d_and_expected[i] = conv_exp.d[i];
    }

  /* ANDC, double */
  for (i = 0; i < 2; i++)
    {
      conv_src_a.d[i] = d_src_a[i];
      conv_src_b.d[i] = d_src_b[i];
      conv_exp.ul[i] = conv_src_a.ul[i] & ~conv_src_b.ul[i];
      d_andc_expected[i] = conv_exp.d[i];
    }

  /* NOR, double */
  for (i = 0; i < 2; i++)
    {
      conv_src_a.d[i] = d_src_a[i];
      conv_src_b.d[i] = d_src_b[i];
      conv_exp.ul[i] = ~(conv_src_a.ul[i] | conv_src_b.ul[i]);
      d_nor_expected[i] = conv_exp.d[i];
    }

  /* OR, double */
  for (i = 0; i < 2; i++)
    {
      conv_src_a.d[i] = d_src_a[i];
      conv_src_b.d[i] = d_src_b[i];
      conv_exp.ul[i] = conv_src_a.ul[i] | conv_src_b.ul[i];
      d_or_expected[i] = conv_exp.d[i];
    }

  /* XOR, double */
  for (i = 0; i < 2; i++)
    {
      conv_src_a.d[i] = d_src_a[i];
      conv_src_b.d[i] = d_src_b[i];
      conv_exp.ul[i] = conv_src_a.ul[i] ^ conv_src_b.ul[i];
      d_xor_expected[i] = conv_exp.d[i];
    }

  /* Run tests.  */
  float_and (f_src_a, f_src_b, f_and_expected);
  float_andc (f_src_a, f_src_b, f_andc_expected);
  float_nor (f_src_a, f_src_b, f_nor_expected);
  float_or (f_src_a, f_src_b, f_or_expected);
  float_xor (f_src_a, f_src_b, f_xor_expected);

  double_and (d_src_a, d_src_b, d_and_expected);
  double_andc (d_src_a, d_src_b, d_andc_expected);
  double_nor (d_src_a, d_src_b, d_nor_expected);
  double_or (d_src_a, d_src_b, d_or_expected);
  double_xor (d_src_a, d_src_b, d_xor_expected);

  return 0;
}

/* { dg-final { scan-assembler-times {\mxxland\M} 2 } } */
/* { dg-final { scan-assembler-times {\mxxlandc\M} 2 } } */
/* { dg-final { scan-assembler-times {\mxxlnor\M} 2 } } */
/* { dg-final { scan-assembler-times {\mxxlxor\M} 2 } } */
/* { dg-final { scan-assembler-times {\mxxlor\M} 22 { target le }} } */
/* { dg-final { scan-assembler-times {\mxxlor\M} 32 { target be }} } */
