/* { dg-do compile } */
/* { dg-additional-options "-march=rv32gcv -mabi=ilp32d --param riscv-autovec-preference=scalable -fno-vect-cost-model -fno-tree-loop-distribute-patterns" } */

#include "single_rgroup-1.h"

TEST_ALL (test_1)

/* { dg-final { scan-assembler-times {vsetvli} 10 } } */
