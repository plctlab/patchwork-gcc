/* { dg-do compile } */
/* Verify that fence mappings match Table A.6's recommended mapping.  */
/* { dg-final { scan-assembler "fence.tso" } } */

int main() {
  __atomic_thread_fence(__ATOMIC_ACQ_REL);
}
