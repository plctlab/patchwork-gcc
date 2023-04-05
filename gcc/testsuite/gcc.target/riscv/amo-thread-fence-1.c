/* { dg-do compile } */
/* { dg-final { scan-assembler-not "fence\t" } } */

int main() {
  __atomic_thread_fence(__ATOMIC_RELAXED);
}
