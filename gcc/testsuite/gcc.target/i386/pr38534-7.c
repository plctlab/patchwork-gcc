/* { dg-do assemble { target *-*-linux* *-*-gnu* } } */
/* { dg-options "-save-temps -march=tigerlake -O2 -mtune-ctrl=^prologue_using_move,^epilogue_using_move" } */

#include "pr38534-1.c"

/* { dg-final { scan-assembler-not "push" } } */
/* { dg-final { scan-assembler-not "pop" } } */
/* { dg-final { scan-assembler-times ".cfi_undefined rbx" 1 { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-times ".cfi_undefined rbp" 1 { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-times ".cfi_undefined r12" 1 { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-times ".cfi_undefined r13" 1 { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-times ".cfi_undefined r14" 1 { target { ! ia32 } } } } */
/* { dg-final { scan-assembler-times ".cfi_undefined r15" 1 { target lp64 } } } */
/* { dg-final { scan-assembler-not ".cfi_undefined r15" { target x32 } } } */
/* { dg-final { scan-assembler-times ".cfi_undefined ebx" 1 { target ia32 } } } */
/* { dg-final { scan-assembler-times ".cfi_undefined esi" 1 { target ia32 } } } */
/* { dg-final { scan-assembler-times ".cfi_undefined edi" 1 { target ia32 } } } */
/* { dg-final { scan-assembler-times ".cfi_undefined ebp" 1 { target ia32 } } } */
