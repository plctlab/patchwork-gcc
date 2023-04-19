/* { dg-do compile } */
/* { dg-options "-march=rv32gc_zve64f_zvl128b -mabi=ilp32d --param riscv-autovec-preference=fixed-vlmax -O3 -ftree-vectorize -fdump-tree-vect-details -save-temps" } */

#include "template-1.h"

/* { dg-final { scan-tree-dump-times "vectorized 1 loops in function" 4 "vect" } } */
