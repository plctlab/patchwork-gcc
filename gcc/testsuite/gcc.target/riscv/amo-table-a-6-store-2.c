/* { dg-do compile } */
/* Verify that fence mappings match Table A.6's recommended mapping.  */
/* { dg-final { scan-assembler "fence\trw,w" } } */
/* { dg-final { scan-assembler-not "fence\trw,rw" } } */
/* { dg-final { scan-assembler-not "fence.tso" } } */

void
foo (int* bar, int* baz) {
  __atomic_store(bar, baz, __ATOMIC_RELEASE);
}
