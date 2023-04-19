/* { dg-do compile } */
/* { dg-options "-march=rv32gcv -mabi=ilp32d --param riscv-autovec-preference=fixed-vlmax -O3 -ftree-vectorize -fdump-tree-vect-details -save-temps" } */

#include "template-1.h"

/* { dg-final { scan-tree-dump-times "vectorized 1 loops in function" 5 "vect" } } */
