/* { dg-do compile } */
/* { dg-options "-march=rv32gc_zve32f_zvl128b -mabi=ilp32d --param riscv-autovec-preference=scalable -O3 -ftree-vectorize -fdump-tree-vect-details -save-temps" } */

#include "template-1.h"
