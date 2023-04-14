/* { dg-do compile } */
/* Verify that fence mappings match Table A.6's recommended mapping.  */
/* { dg-final { scan-assembler "amoadd.w.aqrl\t" } } */

void
foo (int* bar, int* baz) {
  __atomic_add_fetch(bar, baz, __ATOMIC_ACQ_REL);
}
