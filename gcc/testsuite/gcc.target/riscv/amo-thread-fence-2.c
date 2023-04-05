/* { dg-do compile } */
/* { dg-final { scan-assembler "fence\tr,rw" } } */

int main() {
  __atomic_thread_fence(__ATOMIC_ACQUIRE);
}
