/* { dg-do compile } */
/* { dg-options "-mtune=thead-c906" } */

int main () {

/* thead-c906 default is cpu tune param unaligned access fast.  */
#if defined(__riscv_strict_align)
#error "__riscv_strict_align"
#endif

  return 0;
}
