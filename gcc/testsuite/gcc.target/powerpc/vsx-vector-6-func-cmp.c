/* { dg-do run { target lp64 } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } } */
/* { dg-options "-O2 -save-temps" } */

/* Functional test of the vector compare builtins.  */

#include <altivec.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0

#define FLOAT_TEST(NAME)                                                    \
  void __attribute__ ((noipa))                                              \
  float_##NAME (vector float f_src_a, vector float f_src_b, 	            \
		vector bool int f_##NAME##_expected)			    \
  {                                                                         \
    vector bool int f_result = vec_##NAME (f_src_a, f_src_b);		    \
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
	    printf("   expected[0] = 0x%x; result[0] =0x%x\n",		    \
		   f_##NAME##_expected[0], f_result[0]);		    \
	    printf("   expected[1] = 0x%x; result[1] = 0x%x\n",		    \
		   f_##NAME##_expected[1], f_result[1]);		    \
	    printf("   expected[2] = 0x%x; result[2] = 0x%x\n",		    \
		   f_##NAME##_expected[2], f_result[2]);		    \
	    printf("   expected[3] = 0x%x; result[3] = 0x%x\n",		    \
		   f_##NAME##_expected[3], f_result[3]);		    \
	  }								    \
	else								    \
	  abort();							    \
      }                                                                     \
  }

FLOAT_TEST (cmpeq)
FLOAT_TEST (cmpgt)
FLOAT_TEST (cmpge)
FLOAT_TEST (cmplt)
FLOAT_TEST (cmple)

#define DOUBLE_TEST(NAME)                                                    \
  void __attribute__ ((noipa))                                               \
  double_##NAME (vector double d_src_a, vector double d_src_b,               \
		 vector bool long long d_##NAME##_expected)       	     \
  {								             \
    vector bool long long d_result = vec_##NAME (d_src_a, d_src_b);          \
                                                                             \
    if ((d_result[0] != d_##NAME##_expected[0])				     \
	|| (d_result[1] != d_##NAME##_expected[1]))			     \
      {									     \
	if (DEBUG)							     \
	  {								     \
	    printf("ERROR: vec_%s (double) expected value does not match\n", \
		   #NAME);						     \
	    printf("   expected[0] = 0x%lx; result[0] = 0x%lx\n",	     \
		   d_##NAME##_expected[0], d_result[0]);		     \
	    printf("   expected[1] = 0x%lx; result[1] = 0x%lx\n",	     \
		   d_##NAME##_expected[1], d_result[1]);		     \
	  }								     \
	else								     \
	  abort();							     \
	}                                                                    \
    }

DOUBLE_TEST (cmpeq)
DOUBLE_TEST (cmpgt)
DOUBLE_TEST (cmpge)
DOUBLE_TEST (cmplt)
DOUBLE_TEST (cmple)

void abort (void);


int
main () {
  int i;
  vector float f_src_a = { 126.0, 23.0, -338.0, 17.0};
  vector float f_src_b = { 2.00, 23.0, 1.0, 4.0};
  vector bool int f_cmpeq_expected = {0x0, 0xFFFFFFFF, 0x0, 0x0};
  vector bool int f_cmpgt_expected = {0xFFFFFFFF, 0x0, 0x0, 0xFFFFFFFF};
  vector bool int f_cmpge_expected = {0xFFFFFFFF, 0xFFFFFFFF, 0x0, 0xFFFFFFFF};
  vector bool int f_cmplt_expected = {0x0, 0x0, 0xFFFFFFFF, 0x0}; 
  vector bool int f_cmple_expected = {0x0, 0xFFFFFFFF, 0xFFFFFFFF, 0x0}; 

  vector double d_src_a = { 125.44, -338.56};
  vector double d_src_b = { 4.0, -338.56};
  vector bool long long d_cmpeq_expected = {0x0, 0xFFFFFFFFFFFFFFFF};
  vector bool long long d_cmpgt_expected = {0xFFFFFFFFFFFFFFFF, 0x0};
  vector bool long long d_cmpge_expected = {0xFFFFFFFFFFFFFFFF,
					    0xFFFFFFFFFFFFFFFF};
  vector bool long long d_cmplt_expected = {0x0, 0x0};
  vector bool long long d_cmple_expected = {0x0, 0xFFFFFFFFFFFFFFFF};

  float_cmpeq (f_src_a, f_src_b, f_cmpeq_expected);
  float_cmpgt (f_src_a, f_src_b, f_cmpgt_expected);
  float_cmpge (f_src_a, f_src_b, f_cmpge_expected);
  float_cmplt (f_src_a, f_src_b, f_cmplt_expected);
  float_cmple (f_src_a, f_src_b, f_cmple_expected);
 
  double_cmpeq (d_src_a, d_src_b, d_cmpeq_expected);
  double_cmpgt (d_src_a, d_src_b, d_cmpgt_expected);
  double_cmpge (d_src_a, d_src_b, d_cmpge_expected);
  double_cmplt (d_src_a, d_src_b, d_cmplt_expected);
  double_cmple (d_src_a, d_src_b, d_cmple_expected);
  return 0;
}

/* { dg-final { scan-assembler-times {\mxvcmpeqsp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvcmpgtsp\M} 2 } } */
/* { dg-final { scan-assembler-times {\mxvcmpgesp\M} 2 } } */
/* { dg-final { scan-assembler-times {\mxvcmpeqdp\M} 1 } } */
/* { dg-final { scan-assembler-times {\mxvcmpgedp\M} 2 } } */
/* { dg-final { scan-assembler-times {\mxvcmpgtdp\M} 2 } } */
