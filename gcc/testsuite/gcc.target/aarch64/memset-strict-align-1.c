/* { dg-do compile } */
/* { dg-options "-O2 -mstrict-align" } */

struct s { char x[207]; };
struct s s1 __attribute__((aligned(16)));
void foo (struct s *);
void bar (void) { s1 = (struct s){}; foo (&s1); }

/* memset (s1 = {}, sizeof = 207) should be expanded out
   such that there are no overlap stores when -mstrict-align
   is in use.
   so 6 pairs of 16 bytes stores (96 bytes).
   1 8 byte store
   1 4 byte store
   1 2 byte store
   1 1 byte store
   */

/* { dg-final { scan-assembler-times "stp\tq" 6 } } */
/* { dg-final { scan-assembler-times "str\tq" 0 } } */
/* { dg-final { scan-assembler-times "str\txzr" 1 } } */
/* { dg-final { scan-assembler-times "str\twzr" 1 } } */
/* { dg-final { scan-assembler-times "strh\twzr" 1 } } */
/* { dg-final { scan-assembler-times "strb\twzr" 1 } } */

/* No store pair with 8 byte words is needed as foo is called with a sibcall. */
/* { dg-final { scan-assembler-times "stp\tx" 0 } } */
