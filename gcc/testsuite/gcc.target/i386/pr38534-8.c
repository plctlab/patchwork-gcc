/* { dg-do compile { target cfi } } */
/* { dg-options "-march=tigerlake -O2 -mtune-ctrl=^prologue_using_move,^epilogue_using_move" } */

void
__attribute__((noreturn))
no_return_to_caller (void)
{
  while (1);
}

/* { dg-final { scan-assembler-not "push" } } */
/* { dg-final { scan-assembler-not "pop" } } */
/* { dg-final { scan-assembler-not ".cfi_undefined" } } */
