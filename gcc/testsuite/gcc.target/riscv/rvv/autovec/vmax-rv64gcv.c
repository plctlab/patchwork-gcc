/* { dg-do run { target { riscv_vector } } } */
/* { dg-additional-options "-std=c99 -fno-vect-cost-model -march=rv64gcv --param=riscv-autovec-preference=fixed-vlmax -save-temps" } */

#include "vmax-template.h"
#include "vmax-run-template.h"

/* { dg-final { scan-assembler-times {\tvmax\.vv} 6 } } */
/* { dg-final { scan-assembler-times {\tvmaxu\.vv} 6 } } */
