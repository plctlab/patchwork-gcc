/* { dg-do compile } */
/* { dg-final { scan-assembler "fence\trw,w" } } */

int main() {
  __atomic_thread_fence(__ATOMIC_RELEASE);
}
