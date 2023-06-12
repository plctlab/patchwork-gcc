/* { dg-do compile { target { lp64 } } } */
/* { dg-options "-O1 -march=z13 -mzarch" } */
/* { dg-final { scan-assembler-not {\tclc\t} } } */

struct s
{
  long a;
  unsigned b : 1;
  unsigned c : 1;
};

int foo (struct s *x)
{
  /* Expression
       x->b || x->c
     is transformed into
       _1 = BIT_FIELD_REF <*x_4(D), 64, 64>;
       _2 = _1 > 0x3FFFFFFFFFFFFFFF;
     where the constant may materialize in the literal pool and an 8 byte CLC
     may be emitted.  Ensure this is not the case.
  */
  return x->b || x->c;
}
