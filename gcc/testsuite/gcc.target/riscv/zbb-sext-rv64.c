/* { dg-do compile } */
/* { dg-options "-march=rv64g_zbb -mabi=lp64d -O2" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-Og" } } */

int sextb32(int x)
{ return (x << 24) >> 24; }

int sexth32(int x)
{ return (x << 16) >> 16; }

/* { dg-final { scan-assembler "sext.b" } } */
/* { dg-final { scan-assembler "sext.h" } } */