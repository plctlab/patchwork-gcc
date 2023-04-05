/* { dg-do compile } */
/* { dg-final { scan-assembler "fence\trw,rw" } } */

int main() {
  __atomic_thread_fence(__ATOMIC_SEQ_CST);
}
