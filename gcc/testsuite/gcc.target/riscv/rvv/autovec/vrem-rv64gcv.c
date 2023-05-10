/* { dg-do run { target { riscv_vector } } } */
/* { dg-additional-options "-std=c99 -fno-vect-cost-model -march=rv64gcv --param=riscv-autovec-preference=fixed-vlmax -save-temps" } */

#include "vrem-template.h"
#include "vrem-run-template.h"

/* TODO: Implement vector type promotion.  We should have 6 vrem.vv here.  */

/* { dg-final { scan-assembler-times {\tvrem\.vv} 5 } } */
/* { dg-final { scan-assembler-times {\tvremu\.vv} 6 } } */
