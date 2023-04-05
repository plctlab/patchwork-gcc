/* { dg-do compile } */
/* { dg-final { scan-assembler "fence.tso" } } */

int main() {
  __atomic_thread_fence(__ATOMIC_ACQ_REL);
}
