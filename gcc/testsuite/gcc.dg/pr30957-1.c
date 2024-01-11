/* { dg-do run { xfail { mmix-*-* } } } */
/* We don't (and don't want to) perform this optimisation on soft-float targets,
   where each addition is a library call.  /
/* { dg-require-effective-target hard_float } */
/* -fassociative-math requires -fno-trapping-math and -fno-signed-zeros. */
/* { dg-options "-O2 -funroll-loops -fassociative-math -fno-trapping-math -fno-signed-zeros -fvariable-expansion-in-unroller -fdump-rtl-loop2_unroll" } */

extern void abort (void);
extern void exit (int);

float __attribute__((noinline))
foo (float d, int n)
{
  unsigned i;
  float accum = d;

  for (i = 0; i < n; i++)
    accum += d;

  return accum;
}

float __attribute__((noinline))
get_minus_zero()
{
  return 0.0 / -5.0;
}

int
main ()
{
  /* The variable expansion in unroll requires option unsafe-math-optimizations
     (aka -fno-signed-zeros, -fno-trapping-math, -fassociative-math
     and -freciprocal-math).

     When loop like above will have expansion after unrolling as below:

     accum_1 += d_1;
     accum_2 += d_2;
     accum_3 += d_3;
     ...

     The accum_1, accum_2 and accum_3 need to be initialized. Given the
     floating-point we have
     +0.0f + -0.0f = +0.0f.

     Thus, we should initialize the accum_* to -0.0 for correctness.  But
     the things become more complicated when no-signed-zeros, as well as VLA
     vectorizer mode which doesn't trigger variable expansion. Then we have:

     Case 1: Trigger variable expansion but target doesn't honor no-signed-zero.
       minus_zero will be -0.0f and foo (minus_zero, 10) will be -0.0f.
     Case 2: Trigger variable expansion but target does honor no-signed-zero.
       minus_zero will be +0.0f and foo (minus_zero, 10) will be +0.0f.
     Case 3: No variable expansion but target doesn't honor no-signed-zero.
       minus_zero will be -0.0f and foo (minus_zero, 10) will be -0.0f.
     Case 4: No variable expansion but target does honor no-signed-zero.
       minus_zero will be +0.0f and foo (minus_zero, 10) will be +0.0f.

     The test case covers above 4 cases for running.
     */
  float minus_zero = get_minus_zero ();
  float a = __builtin_copysignf (1.0, minus_zero);
  float b = __builtin_copysignf (1.0, foo (minus_zero, 10));

  if (a != b)
    abort ();

  exit (0);
}

/* { dg-final { scan-rtl-dump "Expanding Accumulator" "loop2_unroll" { xfail mmix-*-* } } } */
