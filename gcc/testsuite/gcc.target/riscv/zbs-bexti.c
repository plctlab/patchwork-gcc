/* { dg-do compile } */
/* { dg-options "-march=rv64gc_zbs -mabi=lp64 -O2" } */

/* bexti */
#define BIT_NO  4

long
foo0 (long a)
{
  return (a & (1 << BIT_NO)) ? 0 : -1;
}

/* { dg-final { scan-assembler "bexti" } } */
/* { dg-final { scan-assembler "addi" } } */
