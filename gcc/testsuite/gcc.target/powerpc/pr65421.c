/* PR target/65421 */
/* { dg-options "-O2" } */
/* { dg-require-effective-target powerpc_elfv2 } */
/* { dg-require-effective-target has_arch_ppc64 } */

typedef struct FLOATS
{
  double a[3];
} FLOATS;

/* 3 lfd */
FLOATS ret_arg_pt (FLOATS *a){return *a;}
/* { dg-final { scan-assembler-times {\mlfd\M} 3 } } */

/* 3 stfd */
void st_arg (FLOATS a, FLOATS *p) {*p = a;}
/* { dg-final { scan-assembler-times {\mstfd\M} 3 } } */

/* blr */
FLOATS ret_arg (FLOATS a) {return a;}

typedef struct MIX
{
  double a[2];
  long l;
} MIX;

/* std 3 param regs to return slot */
MIX ret_arg1 (MIX a) {return a;}
/* { dg-final { scan-assembler-times {\mstd\M} 3 } } */

/* count insns */
/* { dg-final { scan-assembler-times {(?n)^\s+[a-z]} 13 } } */
