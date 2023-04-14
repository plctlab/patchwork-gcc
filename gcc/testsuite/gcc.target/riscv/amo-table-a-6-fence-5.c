/* { dg-do compile } */
/* Verify that fence mappings match Table A.6's recommended mapping.  */
/* { dg-final { scan-assembler-times "fence\trw,rw" 1 } } */
/* { dg-final { scan-assembler-not "fence.tso" } } */
/* { dg-final { scan-assembler-not "fence\trw,w" } } */
/* { dg-final { scan-assembler-not "fence\tr,rw" } } */

int main() {
  __atomic_thread_fence(__ATOMIC_SEQ_CST);
}
