/* { dg-do compile } */
/* { dg-options "-O2 -fno-unsafe-math-optimizations -fdump-rtl-combine" } */
/* { dg-require-effective-target arm_arch_v7a_neon_multilib { target { arm*-*-* } } } */
/* { dg-additional-options "-march=armv7-a -mfloat-abi=softfp -mfpu=neon" { target { arm*-*-* } } } */


static const double one=1.0;

double
f(double x)
{
  return x*(one+x);
}

/* { dg-final { scan-rtl-dump-not "\\(plus:DF \\(mult:DF" "combine" } } */
