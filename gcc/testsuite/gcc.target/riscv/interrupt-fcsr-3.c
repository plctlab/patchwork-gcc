/* Verify that fcsr instructions are not emitted.  */
/* { dg-do compile } */
/* { dg-options "" } */

extern int foo (void);

void __attribute__ ((interrupt))
sub (void)
{
}

/* { dg-final { scan-assembler-not "frcsr\t" } } */
/* { dg-final { scan-assembler-not "fscsr\t" } } */
