/* { dg-do run { target { riscv_vector } } } */
/* { dg-additional-options "-std=c99 -fno-vect-cost-model -march=rv32gcv -mabi=ilp32d --param=riscv-autovec-preference=fixed-vlmax -save-temps" } */

#include "vsub-template.h"
#include "vsub-run-template.h"

/* { dg-final { scan-assembler-times {\tvsub\.vv} 12 } } */
