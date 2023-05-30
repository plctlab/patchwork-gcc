/* { dg-do compile { target lp64 } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } } */
/* { dg-options "-O2 -mdejagnu-cpu=power7" } */

/* Functional test of the two operand logical vector builtins.  */

#include <altivec.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0

void abort (void);

int
main () {
  int i;

  union conv_t {
    vector float f;
    vector unsigned int u;
    vector double d;
    vector unsigned long ul;
  } conv_result, conv_exp, conv_src_a, conv_src_b;

  
  vector float f_src_a = { 1.0, 2.0, 3.0, 4.0};
  vector float f_src_b = { 1.0, 3.0, -3.0, 2.0};
  vector float f_result;
  vector float f_expected;

  vector double d_src_a = { 8.0, 10.0};
  vector double d_src_b = { 12.0, 2.0};
  vector double d_result;
  vector double d_expected;

  /* AND, float */
  for (i = 0; i < 4; i++)
    {
      conv_src_a.f[i] = f_src_a[i];
      conv_src_b.f[i] = f_src_b[i];
      conv_exp.u[i] = conv_src_a.u[i] & conv_src_b.u[i];
      f_expected[i] = conv_exp.f[i];
    }

  f_result = vec_and (f_src_a, f_src_b);
  if ((f_result[0] != f_expected[0])
      || (f_result[1] != f_expected[1])
      || (f_result[2] != f_expected[2])
      || (f_result[3] != f_expected[3]))
    {
#if DEBUG
      printf("ERROR: vec_and (float) expected value does not match\n");
      for (i = 0; i < 4; i++)
	{
	  conv_result.f[i] = f_result[i];
	  printf("   expected[%d] = 0x%x; result[%d] = 0x%x\n", i,
		 conv_exp.u[i], i, conv_result.u[i]);
	}
#else
  abort();
#endif
    }

  /* ANDC, float */
  for (i = 0; i < 4; i++)
    {
      conv_src_a.f[i] = f_src_a[i];
      conv_src_b.f[i] = f_src_b[i];
      conv_exp.u[i] = conv_src_a.u[i] & ~conv_src_b.u[i];
      f_expected[i] = conv_exp.f[i];
    }

  f_result = vec_andc (f_src_a, f_src_b);

  if ((f_result[0] != f_expected[0])
      || (f_result[1] != f_expected[1])
      || (f_result[2] != f_expected[2])
      || (f_result[3] != f_expected[3]))
    {
#if DEBUG
      printf("ERROR: vec_andc (float) expected value does not match\n");
      for (i = 0; i < 4; i++)
	{
	  conv_result.f[i] = f_result[i];
	  printf("   expected[%d] = 0x%x; result[%d] = 0x%x\n", i,
		 conv_exp.u[i],	 i, conv_result.u[i]);
	}
#else
      abort();
#endif
    }

  /* NOR, max */
  f_result = vec_nor (f_src_a, f_src_b);
  for (i = 0; i < 4; i++)
    {
      conv_src_a.f[i] = f_src_a[i];
      conv_src_b.f[i] = f_src_b[i];
      conv_exp.u[i] = ~(conv_src_a.u[i] | conv_src_b.u[i]);
      f_expected[i] = conv_exp.f[i];
    }

  if ((f_result[0] != f_expected[0])
      || (f_result[1] != f_expected[1])
      || (f_result[2] != f_expected[2])
      || (f_result[3] != f_expected[3]))
    {
#if DEBUG
      printf("ERROR: vec_nor (float) expected value does not match\n");
      for (i = 0; i < 4; i++)
	{
	  conv_result.f[i] = f_result[i];
	  printf("   expected[%d] = 0x%x; result[%d] = 0x%x\n", i,
		 conv_exp.u[i], i, conv_result.u[i]);
	}
#else
      abort();
#endif
    }

  /* OR, float */
  f_result = vec_or (f_src_a, f_src_b);
  for (i = 0; i < 4; i++)
    {
      conv_src_a.f[i] = f_src_a[i];
      conv_src_b.f[i] = f_src_b[i];
      conv_exp.u[i] = conv_src_a.u[i] | conv_src_b.u[i];
      f_expected[i] = conv_exp.f[i];
    }

  if ((f_result[0] != f_expected[0])
      || (f_result[1] != f_expected[1])
      || (f_result[2] != f_expected[2])
      || (f_result[3] != f_expected[3]))
    {
#if DEBUG
      printf("ERROR: vec_or (float) expected value does not match\n");
      for (i = 0; i < 4; i++)
	{
	  conv_result.f[i] = f_result[i];
	  printf("   expected[%d] = 0x%x; result[%d] = 0x%x\n", i,
		 conv_exp.u[i], i, conv_result.u);
	}
#else
      abort();
#endif
    }

  /* XOR, float */
  f_result = vec_xor (f_src_a, f_src_b);
  for (i = 0; i < 4; i++)
    {
      conv_src_a.f[i] = f_src_a[i];
      conv_src_b.f[i] = f_src_b[i];
      conv_exp.u[i] = conv_src_a.u[i] ^ conv_src_b.u[i];
      f_expected[i] = conv_exp.f[i];
    }

  if ((f_result[0] != f_expected[0])
      || (f_result[1] != f_expected[1])
      || (f_result[2] != f_expected[2])
      || (f_result[3] != f_expected[3]))
    {
#if DEBUG
      printf("ERROR: vec_xor (float) expected value does not match\n");
      for (i = 0; i < 4; i++)
	{
	  conv_result.f[i] = f_result[i];
	  printf("   expected[%d] = 0x%x; result[%d] = 0x%x\n", i,
		 conv_exp.u[i], i, conv_result.u);
	}
#else
      abort();
#endif
    }

  /* AND, double */
  d_result = vec_and (d_src_a, d_src_b);
  for (i = 0; i < 2; i++)
    {
      conv_src_a.d[i] = d_src_a[i];
      conv_src_b.d[i] = d_src_b[i];
      conv_exp.ul[i] = conv_src_a.ul[i] & conv_src_b.ul[i];
      d_expected[i] = conv_exp.d[i];
    }

  if ((d_result[0] != d_expected[0]) || (d_result[1] != d_expected[1]))
    {
#if DEBUG
      printf("ERROR: vec_and (double) expected value does not match\n");
      for (i = 0; i < 2; i++)
	{
	  conv_result.d[i] = d_result[i];
	  printf("   expected[%d] = 0x%lx; result[%d] = 0x%lx\n", i,
		 conv_exp.ul, i, conv_result.ul);
	}
#else
      abort();
#endif
    }

  /* ANDC, double */
  d_result = vec_andc (d_src_a, d_src_b);
  for (i = 0; i < 2; i++)
    {
      conv_src_a.d[i] = d_src_a[i];
      conv_src_b.d[i] = d_src_b[i];
      conv_exp.ul[i] = conv_src_a.ul[i] & ~conv_src_b.ul[i];
      d_expected[i] = conv_exp.d[i];
    }

  if ((d_result[0] != d_expected[0]) || (d_result[1] != d_expected[1]))
    {
#if DEBUG
      printf("ERROR: vec_andc (double) expected value does not match\n");
      for (i = 0; i < 4; i++)
	{
	  conv_result.d[i] = d_result[i];
	  printf("   expected[%d] = 0x%lx; result[%d] = 0x%lx\n", i,
		 conv_exp.ul[i], i, conv_result.ul);
	}
#else
      abort();
#endif
    }

  /* NOR, double */
  d_result = vec_nor (d_src_a, d_src_b);
  for (i = 0; i < 2; i++)
    {
      conv_src_a.d[i] = d_src_a[i];
      conv_src_b.d[i] = d_src_b[i];
      conv_exp.ul[i] = ~(conv_src_a.ul[i] | conv_src_b.ul[i]);
      d_expected[i] = conv_exp.d[i];
    }

  if ((d_result[0] != d_expected[0]) || (d_result[1] != d_expected[1]))
    {
#if DEBUG
      printf("ERROR: vec_nor (double) expected value does not match\n");
      for (i = 0; i < 2; i++)
	{
	  conv_result.d[i] = d_result[i];
	  printf("   expected[%d] = 0x%lx; result[%d] = 0x%lx\n", i,
		 conv_exp.ul, i, conv_result.ul);
	}
#else
      abort();
#endif
    }

  /* OR, double */
  d_result = vec_or (d_src_a, d_src_b);
  for (i = 0; i < 2; i++)
    {
      conv_src_a.d[i] = d_src_a[i];
      conv_src_b.d[i] = d_src_b[i];
      conv_exp.ul[i] = conv_src_a.ul[i] | conv_src_b.ul[i];
      d_expected[i] = conv_exp.d[i];
    }

  if ((d_result[0] != d_expected[0]) || (d_result[1] != d_expected[1]))
    {
#if DEBUG
      printf("ERROR: vec_or (double) expected value does not match\n");
      for (i = 0; i < 2; i++)
	{
	  conv_result.f[i] = f_result[i];
	  printf("   expected[%d] = 0x%lx; result[%d] = 0x%lx\n", i,
		 conv_exp.ul, i, conv_result.ul);
	}
#else
      abort();
#endif
    }

  /* XOR, double */
  d_result = vec_xor (d_src_a, d_src_b);
  for (i = 0; i < 2; i++)
    {
      conv_src_a.d[i] = d_src_a[i];
      conv_src_b.d[i] = d_src_b[i];
      conv_exp.ul[i] = conv_src_a.ul[i] ^ conv_src_b.ul[i];
      d_expected[i] = conv_exp.d[i];
    }

  if ((d_result[0] != d_expected[0]) || (d_result[1] != d_expected[1]))
    {
#if DEBUG
      printf("ERROR: vec_xor (double) expected value does not match\n");
      for (i = 0; i < 2; i++)
	{
	  conv_result.d[i] = f_result[i];
	  printf("   expected[%d] = 0x%lx; result[%d] = 0x%lx\n", i,
		 conv_exp.ul, i, conv_result.ul);
	}
#else
      abort();
#endif
    }

  return 0;
}

