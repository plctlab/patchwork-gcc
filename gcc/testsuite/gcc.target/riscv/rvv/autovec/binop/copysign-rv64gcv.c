/* { dg-do compile } */
/* { dg-additional-options "-std=c99 -O3 -fno-vect-cost-model -march=rv64gcv_zvfh -mabi=lp64d --param=riscv-autovec-preference=fixed-vlmax -ffast-math" } */

#include "copysign-template.h"

/* { dg-final { scan-assembler-times {\tvfsgnj\.vv} 6 } } */
/* { dg-final { scan-assembler-times {\tvfsgnjx\.vv} 6 } } */
/* { dg-final { scan-assembler-times {\tvfsgnjn\.vv} 6 } } */
