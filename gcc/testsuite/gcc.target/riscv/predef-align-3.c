/* { dg-do compile } */
/* { dg-options "-mtune=thead-c906 -mstrict-align" } */

int main () {

#if !defined(__riscv_strict_align)
#error "__riscv_strict_align"
#else
#if __riscv_strict_align != 1
#error "__riscv_strict_align != 1"
#endif
#endif

  return 0;
}
