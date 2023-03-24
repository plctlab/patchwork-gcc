/* { dg-do compile } */
/* { dg-options "-march=rv64g -mabi=lp64d -O2" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-Og" } } */

int sexth32_memory(int* x)
{ return (*x << 16) >> 16; }

/* { dg-final { scan-assembler "lh" } } */
