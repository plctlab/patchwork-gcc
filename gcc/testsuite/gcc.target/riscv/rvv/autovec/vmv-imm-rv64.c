/* { dg-do compile } */
/* { dg-additional-options "-std=c99 -march=rv64gcv -fno-vect-cost-model --param=riscv-autovec-preference=scalable -fno-builtin" } */

#include "vmv-imm-template.h"

/* { dg-final { scan-assembler-times "vmv.v.i" 32 } } */
