/* { dg-do compile } */
/* { dg-options "-march=rv64gc_ssaia" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_ssaia" { target { rv32 } } } */

#ifndef __riscv_ssaia
#error Feature macro not defined
#endif

int
foo (int a)
{
  return a;
}
