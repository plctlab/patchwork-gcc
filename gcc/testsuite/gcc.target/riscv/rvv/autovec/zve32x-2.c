/* { dg-do compile } */
/* { dg-options "-march=rv32gc_zve32x -mabi=ilp32d --param riscv-autovec-preference=fixed-vlmax -O3 -ftree-vectorize -fdump-tree-vect-details -save-temps" } */

#include "template-1.h"


