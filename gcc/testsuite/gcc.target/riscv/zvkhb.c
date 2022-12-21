/* { dg-do compile } */
/* { dg-options "-march=rv64gc_zvkhb" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_zvkhb" { target { rv32 } } } */

#ifndef __riscv_zvkhb
#error Feature macro not defined
#endif

int
foo (int a)
{
  return a;
}
