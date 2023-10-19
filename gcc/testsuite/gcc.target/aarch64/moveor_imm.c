/* { dg-do assemble } */
/* { dg-options "-O2 --save-temps" } */

long f1 (void)
{
  return 0x2aaaaaaaaaaaaaab;
}

long f2 (void)
{
  return 0x10f0f0f0f0f0f0f1;
}

long f3 (void)
{
  return 0xccccccccccccccd;
}

long f4 (void)
{
  return 0x1999999999999998;
}

long f5 (void)
{
  return 0x3f3333333f333333;
}

/* { dg-final { scan-assembler-not {\tmovk\t} } } */
/* { dg-final { scan-assembler-times {\tmov\t} 5 } } */
/* { dg-final { scan-assembler-times {\teor\t} 5 } } */
