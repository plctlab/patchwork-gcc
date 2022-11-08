/* { dg-do compile } */
/* { dg-options "-march=rv64gc -mabi=lp64d" } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-Og" } } */

int
ceil1(float i)
{
  return __builtin_lceil(i);
}

long
ceil2(float i)
{
  return __builtin_lceil(i);
}

long long
ceil3(float i)
{
  return __builtin_lceil(i);
}

int
ceil4(double i)
{
  return __builtin_lceil(i);
}

long
ceil5(double i)
{
  return __builtin_lceil(i);
}

long long
ceil6(double i)
{
  return __builtin_lceil(i);
}

int
floor1(float i)
{
  return __builtin_lfloor(i);
}

long
floor2(float i)
{
  return __builtin_lfloor(i);
}

long long
floor3(float i)
{
  return __builtin_lfloor(i);
}

int
floor4(double i)
{
  return __builtin_lfloor(i);
}

long
floor5(double i)
{
  return __builtin_lfloor(i);
}

long long
floor6(double i)
{
  return __builtin_lfloor(i);
}

/* { dg-final { scan-assembler-times "fcvt.l.s" 6 } } */
/* { dg-final { scan-assembler-times "fcvt.l.d" 6 } } */
/* { dg-final { scan-assembler-not "call" } } */
