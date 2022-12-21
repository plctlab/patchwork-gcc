/* { dg-do compile } */
/* { dg-options "-march=rv64gc_zvkn" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_zvkn" { target { rv32 } } } */

#ifndef __riscv_zvkn
#error Feature macro not defined
#endif

int
foo (int a)
{
  return a;
}
