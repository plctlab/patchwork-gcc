/* { dg-do compile { target cfi } } */
/* { dg-options "-march=tigerlake -O2 -mtune-ctrl=^prologue_using_move,^epilogue_using_move" } */

__attribute__ ((no_callee_saved_registers))
void
foo (void)
{
}

/* { dg-final { scan-assembler-not "push" } } */
/* { dg-final { scan-assembler-not "pop" } } */
/* { dg-final { scan-assembler-not ".cfi_undefined" } } */
