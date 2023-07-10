/* { dg-do compile } */
/* { dg-require-effective-target sse2 } */
/* { dg-options "-O1" } */
/* { dg-final { scan-assembler-not {mm[0-9], [0-9]*\(%rsp\)} } } */

extern int nonsse (int) __attribute__((nosseclobber));
extern int othernonsse (int) __attribute__((nosseclobber));

/* Demonstrate that calling a nosseclobber function from a nosseclobber
   function does _not_ need to save all the regs (unlike in nonsse).  */
__attribute__((nosseclobber)) int nonsse (int i)
{
  return othernonsse (i + 2) + 3;
}
