/* { dg-do compile } */
/* { dg-skip-if "-march given" { *-*-* } { "-march=*" } } */
/* { dg-options "-mcpu=thead-c906" { target { rv64 } } } */
/* T-Head XuanTie C906 => rv64imafdc */

#if !((__riscv_xlen == 64)		\
      && !defined(__riscv_32e)		\
      && defined(__riscv_mul)		\
      && defined(__riscv_atomic)	\
      && (__riscv_flen == 64)		\
      && defined(__riscv_compressed))
#error "unexpected arch"
#endif

int main()
{
  return 0;
}
