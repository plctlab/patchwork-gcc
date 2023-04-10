/* { dg-do compile } */
/* Verify that fence mappings match Table A.6's recommended mapping.  */
/* { dg-final { scan-assembler-not "fence\t" } } */
/* { dg-final { scan-assembler-not "fence.tso" } } */


int main() {
  __atomic_thread_fence(__ATOMIC_RELAXED);
}
