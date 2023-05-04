/* { dg-do compile } */
/* { dg-options "-march=rv32gc_zve64d -mabi=ilp32d --param riscv-autovec-preference=fixed-vlmax -O3 -ftree-vectorize -fdump-tree-vect-details -save-temps" } */

#include "template-1.h"
