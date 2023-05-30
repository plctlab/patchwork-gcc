/* { dg-do compile { target lp64 } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } } */
/* { dg-options "-O2 -mdejagnu-cpu=power7" } */

/* Functional test of the vector compare builtins.  */

#include <altivec.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0

void abort (void);

int
main () {
  int i;
  vector float f_src_a = { 126.0, 23.0, -338.0, 17.0};
  vector float f_src_b = { 2.00, 23.0, 1.0, 4.0};
  vector bool int f_result;
  vector bool int f_cmpeq_expected = {0x0, 0xFFFFFFFF, 0x0, 0x0};
  vector bool int f_cmpgt_expected = {0xFFFFFFFF, 0x0, 0x0, 0xFFFFFFFF};
  vector bool int f_cmpge_expected = {0xFFFFFFFF, 0xFFFFFFFF, 0x0, 0xFFFFFFFF};
  vector bool int f_cmplt_expected = {0x0, 0x0, 0xFFFFFFFF, 0x0}; 
  vector bool int f_cmple_expected = {0x0, 0xFFFFFFFF, 0xFFFFFFFF, 0x0}; 

  vector double d_src_a = { 125.44, -338.56};
  vector double d_src_b = { 4.0, -338.56};
  vector bool long long d_result;
  vector bool long long d_cmpeq_expected = {0x0, 0xFFFFFFFFFFFFFFFF};
  vector bool long long d_cmpgt_expected = {0xFFFFFFFFFFFFFFFF, 0x0};
  vector bool long long d_cmpge_expected = {0xFFFFFFFFFFFFFFFF,
					    0xFFFFFFFFFFFFFFFF};
  vector bool long long d_cmplt_expected = {0x0, 0x0};
  vector bool long long d_cmple_expected = {0x0, 0xFFFFFFFFFFFFFFFF};

  /* Compare equal, float */
  f_result = vec_cmpeq (f_src_a, f_src_b);

  if ((f_result[0] != f_cmpeq_expected[0])
      || (f_result[1] != f_cmpeq_expected[1])
      || (f_result[2] != f_cmpeq_expected[2])
      || (f_result[3] != f_cmpeq_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_cmpeq (float) expected value does not match\n");
      printf("   expected[0] = 0x%x; result[0] =0x%x\n",
	     f_cmpeq_expected[0], f_result[0]);
      printf("   expected[1] = 0x%x; result[1] = 0x%x\n",
	     f_cmpeq_expected[1], f_result[1]);
      printf("   expected[2] = 0x%x; result[2] = 0x%x\n",
	     f_cmpeq_expected[2], f_result[2]);
      printf("   expected[3] = 0x%x; result[3] = 0x%x\n",
	     f_cmpeq_expected[3], f_result[3]);
    }
#else
  abort();
#endif

  /* Compare greater than, float */
  f_result = vec_cmpgt (f_src_a, f_src_b);

  if ((f_result[0] != f_cmpgt_expected[0])
      || (f_result[1] != f_cmpgt_expected[1])
      || (f_result[2] != f_cmpgt_expected[2])
      || (f_result[3] != f_cmpgt_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_cmpgt (float) expected value does not match\n");
      printf("   expected[0] = 0x%x; result[0] = 0x%x\n",
	     f_cmpgt_expected[0], f_result[0]);
      printf("   expected[1] = 0x%x; result[1] = 0x%x\n",
	     f_cmpgt_expected[1], f_result[1]);
      printf("   expected[2] = 0x%x; result[2] = 0x%x\n",
	     f_cmpgt_expected[2], f_result[2]);
      printf("   expected[3] = 0x%x; result[3] = 0x%x\n",
	     f_cmpgt_expected[3], f_result[3]);
    }
#else
  abort();
#endif

  /* Compare greater than or equal, float */
  f_result = vec_cmpge (f_src_a, f_src_b);

  if ((f_result[0] != f_cmpge_expected[0])
      || (f_result[1] != f_cmpge_expected[1])
      || (f_result[2] != f_cmpge_expected[2])
      || (f_result[3] != f_cmpge_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_cmpge(float) expected value does not match\n");
      printf("   expected[0] = 0x%x; result[0] = 0x%x\n",
	     f_cmpge_expected[0], f_result[0]);
      printf("   expected[1] = 0x%x; result[1] = 0x%x\n",
	     f_cmpge_expected[1], f_result[1]);
      printf("   expected[2] = 0x%x; result[2] = 0x%x\n",
	     f_cmpge_expected[2], f_result[2]);
      printf("   expected[3] = 0x%x; result[3] = 0x%x\n",
	     f_cmpge_expected[3], f_result[3]);
    }
#else
  abort();
#endif

  /* Compare less than, float */
  f_result = vec_cmplt (f_src_a, f_src_b);

  if ((f_result[0] != f_cmplt_expected[0])
      || (f_result[1] != f_cmplt_expected[1])
      || (f_result[2] != f_cmplt_expected[2])
      || (f_result[3] != f_cmplt_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_cmplt (float) expected value does not match\n");
      printf("   expected[0] = 0x%x; result[0] = 0x%x\n",
	     f_cmplt_expected[0], f_result[0]);
      printf("   expected[1] = 0x%x; result[1] = 0x%x\n",
	     f_cmplt_expected[1], f_result[1]);
      printf("   expected[2] = 0x%x; result[2] = 0x%x\n",
	     f_cmplt_expected[2], f_result[2]);
      printf("   expected[3] = 0x%x; result[3] = 0x%x\n",
	     f_cmplt_expected[3], f_result[3]);
    }
#else
  abort();
#endif

  /* Compare less than or equal, float */
  f_result = vec_cmple (f_src_a, f_src_b);

  if ((f_result[0] != f_cmple_expected[0])
      || (f_result[1] != f_cmple_expected[1])
      || (f_result[2] != f_cmple_expected[2])
      || (f_result[3] != f_cmple_expected[3]))
#if DEBUG
    {
      printf("ERROR: vec_cmple (float) expected value does not match\n");
      printf("   expected[0] = 0x%x; result[0] = 0x%x\n",
	     f_cmple_expected[0], f_result[0]);
      printf("   expected[1] = 0x%x; result[1] = 0x%x\n",
	     f_cmple_expected[1], f_result[1]);
      printf("   expected[2] = 0x%x; result[2] = 0x%x\n",
	     f_cmple_expected[2], f_result[2]);
      printf("   expected[3] = 0x%x; result[3] = 0x%x\n",
	     f_cmple_expected[3], f_result[3]);
    }
#else
  abort();
#endif


  /* Compare equal, double */
  d_result = vec_cmpeq (d_src_a, d_src_b);

  if ((d_result[0] != d_cmpeq_expected[0]) || (d_result[1] != d_cmpeq_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_cmpeq (double) expected value does not match\n");
      printf("   expected[0] = 0x%lx; result[0] = 0x%lx\n",
	     d_cmpeq_expected[0], d_result[0]);
      printf("   expected[1] = 0x%lx; result[1] = 0x%lx\n",
	     d_cmpeq_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  /* Compare greater than, double */
  d_result = vec_cmpgt (d_src_a, d_src_b);

  if ((d_result[0] != d_cmpgt_expected[0])
      || (d_result[1] != d_cmpgt_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_cmpgt (double) expected value does not match\n");
      printf("   expected[0] = 0x%lx; result[0] = 0x%lx\n",
	     d_cmpgt_expected[0], d_result[0]);
      printf("   expected[1] = 0x%lx; result[1] = 0x%lx\n",
	     d_cmpgt_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  /* Compare greater than or equal, double */
  d_result = vec_cmpge (d_src_a, d_src_b);

  if ((d_result[0] != d_cmpge_expected[0])
      || (d_result[1] != d_cmpge_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_cmpge (double) expected value does not match\n");
      printf("   expected[0] = 0x%lx; result[0] = 0x%lx\n",
	     d_cmpgt_expected[0], d_result[0]);
      printf("   expected[1] = 0x%lx; result[1] = 0x%lx\n",
	     d_cmpgt_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  /* Compare less than, double */
  d_result = vec_cmplt (d_src_a, d_src_b);

  if ((d_result[0] != d_cmplt_expected[0])
      || (d_result[1] != d_cmplt_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_cmplt (double) expected value does not match\n");
      printf("   expected[0] = 0x%lx; result[0] = 0x%lx\n",
	     d_cmplt_expected[0], d_result[0]);
      printf("   expected[1] = 0x%lx; result[1] = 0x%lx\n",
	     d_cmplt_expected[1], d_result[1]);
    }
#else
  abort();
#endif

  /* Compare less than or equal, double */
  d_result = vec_cmple (d_src_a, d_src_b);

  if ((d_result[0] != d_cmple_expected[0])
      || (d_result[1] != d_cmple_expected[1]))
#if DEBUG
    {
      printf("ERROR: vec_cmple (double) expected value does not match\n");
      printf("   expected[0] = 0x%lx; result[0] = 0x%lx\n",
	     d_cmple_expected[0], d_result[0]);
      printf("   expected[1] = 0x%lx; result[1] = 0x%lx\n",
	     d_cmple_expected[1], d_result[1]);
    }
#else
  abort();
#endif
  return 0;
}
