/* { dg-do run { target { riscv_vector } } } */
/* { dg-additional-options "-std=c99 -fno-vect-cost-model -march=rv32gcv -mabi=ilp32d --param=riscv-autovec-preference=fixed-vlmax -save-temps" } */

#include "vdiv-template.h"
#include "vdiv-run-template.h"

/* TODO: Implement vector type promotion.  We should have 6 vdiv.vv here.  */

/* { dg-final { scan-assembler-times {\tvdiv\.vv} 4 } } */
/* { dg-final { scan-assembler-times {\tvdivu\.vv} 6 } } */
