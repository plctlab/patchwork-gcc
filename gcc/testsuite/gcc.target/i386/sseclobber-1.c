/* { dg-do compile } */
/* { dg-require-effective-target sse2 } */
/* { dg-options "-O1" } */
/* { dg-final { scan-assembler-times {mm[89], [0-9]*\(%rsp\)} 2 } } */
/* { dg-final { scan-assembler-times {mm1[0-5], [0-9]*\(%rsp\)} 6 } } */

extern int nonsse (int) __attribute__((nosseclobber));
extern int normalfunc (int);

/* Demonstrate that all regs potentially clobbered by normal psABI
   functions are saved/restored by otherabi functions.  */
__attribute__((nosseclobber)) int nonsse (int i)
{
  return normalfunc (i + 2) + 3;
}
