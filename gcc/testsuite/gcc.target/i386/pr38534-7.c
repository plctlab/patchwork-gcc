/* { dg-do assemble { target *-*-linux* *-*-gnu* } } */
/* { dg-options "-save-temps -march=tigerlake -O2 -mtune-ctrl=^prologue_using_move,^epilogue_using_move" } */

#include "pr38534-1.c"

/* { dg-final { scan-assembler-not "push" } } */
/* { dg-final { scan-assembler-not "pop" } } */
/* { dg-final { scan-assembler-times ".cfi_undefined 3" 1 { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-times ".cfi_undefined 6" 1 { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-times ".cfi_undefined 12" 1 { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-times ".cfi_undefined 13" 1 { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-times ".cfi_undefined 14" 1 { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-times ".cfi_undefined 15" 1 { target lp64 } } } */
/* { dg-final { scan-assembler-not ".cfi_undefined 15" { target x32 } } } */
/* { dg-final { scan-assembler-times ".cfi_undefined 3" 1 { target ia32 } } } */
/* { dg-final { scan-assembler-times ".cfi_undefined 5" 1 { target ia32 } } } */
/* { dg-final { scan-assembler-times ".cfi_undefined 6" 1 { target ia32 } } } */
/* { dg-final { scan-assembler-times ".cfi_undefined 7" 1 { target ia32 } } } */
