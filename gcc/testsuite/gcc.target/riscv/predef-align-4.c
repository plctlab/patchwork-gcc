/* { dg-do compile } */
/* { dg-options "-mtune=rocket" } */

int main () {

/* rocket default is cpu tune param unaligned access slow.  */
#if !defined(__riscv_strict_align)
#error "__riscv_strict_align"
#else
#if __riscv_strict_align != 2
#error "__riscv_strict_align != 2"
#endif
#endif

  return 0;
}
