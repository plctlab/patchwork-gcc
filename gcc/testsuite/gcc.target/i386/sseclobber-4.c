/* { dg-do compile } */
/* { dg-require-effective-target sse2 } */
/* { dg-options "-O1" } */
/* { dg-final { scan-assembler-not {mm[0-9], [0-9]*\(%rsp\)} } } */

extern __attribute__((nosseclobber)) int (*nonsse_ptr) (int);

/* Demonstrate that some values can be kept in a register over calls
   to otherabi functions when called via function pointer.  */
double docalc (double d)
{
  double ret = d;
  int i = 0;
  while (1) {
      int j = nonsse_ptr (i++);
      if (!j)
        break;
      ret += j;
  }
  return ret;
}
