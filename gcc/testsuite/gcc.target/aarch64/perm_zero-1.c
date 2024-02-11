/* { dg-do compile } */
/* { dg-options "-O2"  } */
/* PR target/113872 */
/* For 64bit vectors, PERM with a constant 0 should produce a shift instead of the ext instruction. */

#define vect64 __attribute__((vector_size(8)))

void f(vect64  unsigned short *a)
{
  *a = __builtin_shufflevector((vect64 unsigned short){0},*a, 3,4,5,6);
}

/* { dg-final { scan-assembler-times "ushr\t" 1 { target aarch64_big_endian } } } */
/* { dg-final { scan-assembler-times "shl\t" 1 { target aarch64_little_endian } } } */
/* { dg-final { scan-assembler-not "ext\t"  } } */
