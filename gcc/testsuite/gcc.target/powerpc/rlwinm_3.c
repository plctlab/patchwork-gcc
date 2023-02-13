/* { dg-do run } */
/* { dg-options "-O2 -save-temps" } */

typedef unsigned long long u64;
typedef unsigned int u32;
#define NOINLE __attribute__ ((noinline))
#define V (0x9753)

#define MASK 0xffffffffe0000003ULL
#define N 4
#define LMASK 0xfffff00

u64 NOINLE
test_rlwinm_lowpart_mask (u32 v)
{
  u32 v1 = ((v << N) | (v >> (32 - N))) & LMASK;
  return (u64)v1;
}

u64 NOINLE
test_rlwinm_mask (u32 v)
{
  u32 v1 = ((v << N) | (v >> (32 - N)));
  u64 v2 = (u64) v1 | ((u64) v1 << 32);
  return v2 & MASK;
}

/* { dg-final { scan-assembler-times {\mrlwinm\M} 2 { target has_arch_ppc64 } } } */

#define RLWINM_L(v, n)                                                         \
  ((((v & 0xffffffffLL) << n) | ((v & 0xffffffffLL) >> (32 - n)))              \
   & 0xffffffffLL)
#define RLWINM_MASK(v, n, m) (((RLWINM_L (v, n) << 32) | (RLWINM_L (v, n))) & m)

u64 v_low_mask = RLWINM_MASK (V, N, LMASK);
u64 v_mask = RLWINM_MASK (V, N, MASK);

int
main ()
{
  u64 v = V;
  if (test_rlwinm_lowpart_mask (v) != v_low_mask
      || test_rlwinm_mask (v) != v_mask)
    __builtin_abort ();

  return 0;
}
