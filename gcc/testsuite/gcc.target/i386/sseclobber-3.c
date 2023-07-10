/* { dg-do compile } */
/* { dg-require-effective-target sse2 } */
/* { dg-options "-O1" } */
/* for docalc2 we should use the high xmm regs */
/* { dg-final { scan-assembler {xmm[89]} } } */
/* do docalc4_notany we should use the high ymm regs */
/* { dg-final { scan-assembler {ymm[89]} } } */
/* for docalc4 (and nowhere else) we should save/restore exactly
   one reg to stack around the inner-loop call */
/* { dg-final { scan-assembler-times {ymm[0-9]*, [0-9]*\(%rsp\)} 1 } } */

typedef double dbl2 __attribute__((vector_size(16)));
typedef double dbl4 __attribute__((vector_size(32)));
typedef double dbl8 __attribute__((vector_size(64)));
extern __attribute__((nosseclobber,const)) double nonsse (int);

/* Demonstrate that some values can be kept in a register over calls
   to otherabi functions.  nonsse saves the XMM register, so those
   are usable, hence docalc2 should be able to keep values in registers
   over the nonsse call.  */
void docalc2 (dbl2 *d, dbl2 *a, dbl2 *b, int n)
{
  long i;
  for (i = 0; i < n; i++)
    {
      d[i] = a[i] * b[i] * nonsse(i);
    }
}

/* Here we're using YMM registers (four doubles) and those are _not_
   saved by nonsse() (only the XMM parts) so docalc4 should not keep
   the value in a register over the call to nonsse.  */
void __attribute__((target("avx2"))) docalc4 (dbl4 *d, dbl4 *a, dbl4 *b, int n)
{
  long i;
  for (i = 0; i < n; i++)
    {
      d[i] = a[i] * b[i] * nonsse(i);
    }
}

/* And here we're also using YMM registers, but have a call to a
   noanysseclobber function, which _does_ save all [XYZ]MM regs except
   arguments, so docalc4_notany should again be able to keep the value
   in a register.  */
extern __attribute__((noanysseclobber,const)) double notanysse (int);
void __attribute__((target("avx2"))) docalc4_notany (dbl4 *d, dbl4 *a, dbl4 *b, int n)
{
  long i;
  for (i = 0; i < n; i++)
    {
      d[i] = a[i] * b[i] * notanysse(i);
    }
}
