/* { dg-do compile } */
/* { dg-options "-mtune=rocket -mno-strict-align" } */

int main () {

/* -mno-strict-align override due to cpu tune param.  */
#if !defined(__riscv_strict_align)
#error "__riscv_strict_align"
#else
#if __riscv_strict_align != 2
#error "__riscv_strict_align != 2"
#endif
#endif

  return 0;
}
