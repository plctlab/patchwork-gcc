/* { dg-do compile { target int128 } } */
/* { dg-options "-O2" } */

typedef unsigned long long u64;
typedef unsigned __int128 u128;
void testcase1(u64 *acc, u64 a, u64 b)
{
  u128 res = (u128)a*b;
  u64 lo = res, hi = res >> 64;
  unsigned char cf = 0;
  cf = __builtin_ia32_addcarryx_u64 (cf, lo, acc[0], acc+0);
  cf = __builtin_ia32_addcarryx_u64 (cf, hi, acc[1], acc+1);
  cf = __builtin_ia32_addcarryx_u64 (cf,  0, acc[2], acc+2);
}

/* { dg-final { scan-assembler-times "movq" 1 } } */
