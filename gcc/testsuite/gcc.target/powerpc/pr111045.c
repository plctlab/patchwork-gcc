/* PR target/111045 */
/* { dg-require-effective-target power10_ok } */
/* { dg-options "-O2 -mdejagnu-cpu=power10" } */

/* Verify we do not generate a PCREL access on ABIs that doesn't support it.  */

static unsigned short s;

unsigned short *
foo (void)
{
  return &s;
}

/* { dg-final { scan-assembler-not {\m@pcrel\M} { target { ! powerpc_pcrel } } } } */
