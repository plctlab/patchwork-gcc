/* { dg-do compile } */
/* { dg-options "-fpic" } */

extern int var;

void
func (void)
{
label:
  __asm__ ("@ %0" : : "S" (func));
  __asm__ ("@ %0" : : "S" (&var + 1));
  __asm__ ("@ %0" : : "S" (&&label));
}

/* { dg-final { scan-assembler "@ func" } } */
/* { dg-final { scan-assembler "@ var\\+4" } } */
/* { dg-final { scan-assembler "@ .L" } } */
