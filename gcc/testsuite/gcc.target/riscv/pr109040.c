/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64 -O2 -fno-schedule-insns -fno-schedule-insns2" } */

typedef unsigned short __attribute__((__vector_size__ (32))) V;
typedef unsigned short u16;

void
foo (V m, u16 *ret)
{
  V v = 6 > ((V) { 2049, 8 } & m);
  *ret = v[0];
}

/* { dg-final { scan-assembler-times {slli\s+a[0-9]+,\s*a[0-9]+,\s*48\s+srli\s+a[0-9]+,\s*a[0-9]+,\s*48} 1 } } */
