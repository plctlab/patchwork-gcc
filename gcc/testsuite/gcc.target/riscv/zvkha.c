/* { dg-do compile } */
/* { dg-options "-march=rv64gc_zvkha" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_zvkha" { target { rv32 } } } */

#ifndef __riscv_zvkha
#error Feature macro not defined
#endif

int
foo (int a)
{
  return a;
}
