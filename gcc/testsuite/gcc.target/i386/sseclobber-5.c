/* { dg-do compile } */
/* { dg-require-effective-target sse2 } */
/* { dg-options "-O1" } */
/* { dg-final { scan-assembler-not {mm[89]} } } */
/* { dg-final { scan-assembler-not {mm1[0-5]} } } */

extern int noanysse (int) __attribute__((noanysseclobber));
extern int noanysse2 (int) __attribute__((noanysseclobber));
extern __attribute__((noanysseclobber)) double calcstuff (double, double);

/* Demonstrate that none of the clobbered SSE (or wider) regs are
   used by a noanysse function.  */
__attribute__((noanysseclobber)) double calcstuff (double d, double e)
{
  double s1, s2, s3, s4, s5, s6, s7, s8;
  s1 = s2 = s3 = s4 = s5 = s6 = s7 = s8 = 0.0;
  while (d > 0.1)
    {
      s1 += s2 * 2 + d;
      s2 += s3 * 3 + e;
      s3 += s4 * 5 + d * e;
      s4 += e / d;
      s5 += s2 * 7 + d - e;
      s5 += 2 * d + e;
      s6 += 5 * e + d;
      s7 += 7 * e * (d+1);
      d -= e;
    }
  return s1 + s2 + s3 + s4 + s5 + s6 + s7;
}

/* Demonstrate that we can call noanysse functions from noannysse
   functions.  */
__attribute__((noanysseclobber)) int noanysse2 (int i)
{
  return noanysse (i + 2) + 3;
}
