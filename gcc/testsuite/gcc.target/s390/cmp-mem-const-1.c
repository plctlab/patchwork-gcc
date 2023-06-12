/* { dg-do compile { target { lp64 } } } */
/* { dg-options "-O1 -march=z13 -mzarch" } */
/* { dg-final { scan-assembler-not {\tclc\t} } } */

int
ge_8b (unsigned long *x)
{
  return *x >= 0x4000000000000000;
}

int
ge_8b_adjust (unsigned long *x)
{
  return *x >= 0x4000000000000001;
}

int
ge_6b (unsigned long *x)
{
  return *x >= 0x400000000000;
}

int
ge_6b_adjust (unsigned long *x)
{
  return *x >= 0x400000000001;
}

int
gt_8b (unsigned long *x)
{
  return *x > 0x4000000000000000;
}

int
gt_8b_adjust (unsigned long *x)
{
  return *x > 0x3fffffffffffffff;
}

int
gt_6b (unsigned long *x)
{
  return *x > 0x400000000000;
}

int
gt_6b_adjust (unsigned long *x)
{
  return *x > 0x3fffffffffff;
}

int
le_8b (unsigned long *x)
{
  return *x <= 0x3ff7ffffffffffff;
}

int
le_6b (unsigned long *x)
{
  return *x <= 0x3f7fffffffff;
}

int
le_8b_adjust (unsigned long *x)
{
  return *x <= 0x3ffffdfffffffffe;
}

int
le_6b_adjust (unsigned long *x)
{
  return *x <= 0x3ffffffffffe;
}

int
lt_8b (unsigned long *x)
{
  return *x < 0x3fffffffffffffff;
}

int
lt_6b (unsigned long *x)
{
  return *x < 0x3fffffffffff;
}

int
lt_8b_adjust (unsigned long *x)
{
  return *x < 0x4020000000000000;
}

int
lt_6b_adjust (unsigned long *x)
{
  return *x < 0x400000000000;
}
