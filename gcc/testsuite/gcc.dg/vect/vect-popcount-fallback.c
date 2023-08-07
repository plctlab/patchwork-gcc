/* Check if we vectorize popcount when no expander is available.  */
/* { dg-do run { target { amdgcn-*-* sparc*-*-* alpha*-*-* ia64-*-* mips*-*-* riscv*-*-* } } } */
/* { dg-additional-options { -O2 -fdump-tree-vect-details -fno-vect-cost-model } }  */
/* { dg-require-effective-target vect_int } */

#include <stdlib.h>
#include <assert.h>
#include <stdint-gcc.h>

__attribute__ ((noipa))
void popc32 (uint32_t *restrict dst, uint32_t *restrict a, int n)
{
  for (int i = 0; i < n; i++)
    dst[i] = __builtin_popcount (a[i]);
}

__attribute__ ((noipa))
void ctz32 (uint32_t *restrict dst, uint32_t *restrict a, int n)
{
  for (int i = 0; i < n; i++)
    dst[i] = __builtin_ctz (a[i]);
}

__attribute__ ((noipa))
void ffs32 (uint32_t *restrict dst, uint32_t *restrict a, int n)
{
  for (int i = 0; i < n; i++)
    dst[i] = __builtin_ffs (a[i]);
}

__attribute__ ((noipa))
void popc64 (uint64_t *restrict dst, uint64_t *restrict a, int n)
{
  for (int i = 0; i < n; i++)
    dst[i] = __builtin_popcountll (a[i]);
}

__attribute__ ((noipa))
void ctz64 (uint64_t *restrict dst, uint64_t *restrict a, int n)
{
  for (int i = 0; i < n; i++)
    dst[i] = __builtin_ctzll (a[i]);
}

__attribute__ ((noipa))
void ffs64 (uint64_t *restrict dst, uint64_t *restrict a, int n)
{
  for (int i = 0; i < n; i++)
    dst[i] = __builtin_ffsll (a[i]);
}

#define SZ 512

__attribute__ ((optimize ("0")))
int main ()
{
  uint32_t *a32pc = malloc (SZ * sizeof (*a32pc));
  uint32_t *b32pc = malloc (SZ * sizeof (*b32pc));
  uint32_t *a32ctz = malloc (SZ * sizeof (*a32ctz));
  uint32_t *b32ctz = malloc (SZ * sizeof (*b32ctz));
  uint32_t *a32ffs = malloc (SZ * sizeof (*a32ffs));
  uint32_t *b32ffs = malloc (SZ * sizeof (*b32ffs));

  uint64_t *a64pc = malloc (SZ * sizeof (*a64pc));
  uint64_t *b64pc = malloc (SZ * sizeof (*b64pc));
  uint64_t *a64ctz = malloc (SZ * sizeof (*a64ctz));
  uint64_t *b64ctz = malloc (SZ * sizeof (*b64ctz));
  uint64_t *a64ffs = malloc (SZ * sizeof (*a64ffs));
  uint64_t *b64ffs = malloc (SZ * sizeof (*b64ffs));

  for (int i = 0; i < SZ; i++)
    {
      int ia = i + 1;
      a32pc[i] = ia * 1234567;
      b32pc[i] = 0;
      a32ctz[i] = ia * 1234567;
      b32ctz[i] = 0;
      a32ffs[i] = ia * 1234567;
      b32ffs[i] = 0;
      a64pc[i] = ia * 123456789ull;
      b64pc[i] = 0;
      a64ctz[i] = ia * 123456789ull;
      b64ctz[i] = 0;
      a64ffs[i] = ia * 123456789ull;
      b64ffs[i] = 0;
    }

  popc32 (b32pc, a32pc, SZ);
  ctz32 (b32ctz, a32ctz, SZ);
  ffs32 (b32ffs, a32ffs, SZ);
  popc64 (b64pc, a64pc, SZ);
  ctz64 (b64ctz, a64ctz, SZ);
  ffs64 (b64ffs, a64ffs, SZ);

  for (int i = 0; i < SZ; i++)
    {
      assert (b32pc[i] == __builtin_popcount (a32pc[i]));
      assert (b32ctz[i] == __builtin_ctz (a32ctz[i]));
      assert (b32ffs[i] == __builtin_ffs (a32ffs[i]));
      assert (b64pc[i] == __builtin_popcountll (a64pc[i]));
      assert (b64ctz[i] == __builtin_ctzll (a64ctz[i]));
      assert (b64ffs[i] == __builtin_ffsll (a64ffs[i]));
    }
}

/* { dg-final { scan-tree-dump-times "LOOP VECTORIZED" 6 "vect" target { amdgcn-*-* sparc*-*-* alpha*-*-* ia64-*-* mips*-*-* riscv*-*-* } } }  */
