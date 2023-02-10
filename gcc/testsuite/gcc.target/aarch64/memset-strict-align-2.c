/* { dg-do compile } */
/* { dg-options "-O2 -mstrict-align" } */

struct s { char x[7]; };
void foo (struct s *);
void bar (struct s *s1) { *s1 = (struct s){}; foo (s1); }

/* memset (s1 = {}, sizeof = 7) should be expanded out
   such that there are no overlap stores when -mstrict-align
   is in use. As the alignment of s1 is unknown, byte stores are needed.
   so 15 byte stores
   */

/* { dg-final { scan-assembler-times "strb\twzr" 7 } } */
