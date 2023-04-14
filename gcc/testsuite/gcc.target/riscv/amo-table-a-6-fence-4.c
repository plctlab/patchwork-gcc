/* { dg-do compile } */
/* Verify that fence mappings match Table A.6's recommended mapping.  */
/* { dg-final { scan-assembler-times "fence.tso" 1 } } */
/* { dg-final { scan-assembler-not "fence\trw,w" } } */
/* { dg-final { scan-assembler-not "fence\tr,rw" } } */
/* { dg-final { scan-assembler-not "fence\trw,rw" } } */

int main() {
  __atomic_thread_fence(__ATOMIC_ACQ_REL);
}
