/* { dg-do compile } */
/* { dg-options "-march=rv64g -mabi=lp64d -O2" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-Og" } } */

int sextb32_memory(int* x)
{ return (*x << 24) >> 24; }

/* { dg-final { scan-assembler "lb" } } */
