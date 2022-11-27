/* { dg-do compile } */
/* { dg-options "-march=rv64gc_smaia" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_smaia" { target { rv32 } } } */

#ifndef __riscv_smaia
#error Feature macro not defined
#endif

// Smaia implies Ssaia
#ifndef __riscv_ssaia
#error Feature macro not defined
#endif

int
foo (int a)
{
  return a;
}
