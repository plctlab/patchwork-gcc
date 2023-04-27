/* { dg-do compile } */
/* Verify that store mappings match Table A.6's recommended mapping.  */
/* { dg-final { scan-assembler-times "fence\trw,w" 1 } } */
/* { dg-final { scan-assembler-not "fence\tr,rw" } } */
/* { dg-final { scan-assembler-not "fence\trw,rw" } } */
/* { dg-final { scan-assembler-not "fence.tso" } } */

void
foo (int* bar, int* baz) {
  __atomic_store(bar, baz, __ATOMIC_RELEASE);
}
