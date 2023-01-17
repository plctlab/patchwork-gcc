/* { dg-do compile } */
/* { dg-options "-mtune=thead-c906 -mno-strict-align" } */

int main () {

#if defined(__riscv_strict_align)
#error "__riscv_strict_align"
#endif

  return 0;
}
