/* { dg-do compile } */
/* { dg-options " -march=loongarch64 " } */
/* { dg-final { scan-assembler-times {bstrins.d}  6} } */
/* { dg-final { scan-assembler-not "my_fabsq2:.*\\bl\t%plt\\(__fabsq\\).*my_fabsq2" } } */
/* { dg-final { scan-assembler-not "my_copysignq2:.*\\bl\t%plt\\(__copysignq\\).*my_copysignq2" } } */
/* { dg-final { scan-assembler-not "my_nanq2:.*\\bl\t%plt\\(__builtin_nanq\\).*my_nanq2" } } */
/* { dg-final { scan-assembler-not "my_nansq2:.*\\bl\t%plt\\(__builtin_nansq\\).*my_nansq2" } } */

__float128
my_fabsq1 (__float128 a)
{
  return __builtin_fabsq (a);
}

_Float128
my_fabsq2 (_Float128 a)
{
  return __builtin_fabsq (a);
}

_Float128
my_fabsf128 (_Float128 a)
{
  return __builtin_fabsf128 (a);
}

__float128
my_copysignq1 (__float128 a, __float128 b)
{
  return __builtin_copysignq (a, b);
}

_Float128
my_copysignq2 (_Float128 a, _Float128 b)
{
  return __builtin_copysignq (a, b);
}

_Float128
my_copysignf128 (_Float128 a, _Float128 b)
{
  return __builtin_copysignf128 (a, b);
}

__float128
my_infq1 (void)
{
  return __builtin_infq ();
}

_Float128
my_infq2 (void)
{
  return __builtin_infq ();
}

_Float128
my_inff128 (void)
{
  return __builtin_inff128 ();
}

__float128
my_huge_valq1 (void)
{
  return __builtin_huge_valq ();
}

_Float128
my_huge_valq2 (void)
{
  return __builtin_huge_valq ();
}

_Float128
my_huge_valf128 (void)
{
  return __builtin_huge_valf128 ();
}

__float128
my_nanq1 (void)
{
  return __builtin_nanq ("");
}

_Float128
my_nanq2 (void)
{
  return __builtin_nanq ("");
}

_Float128
my_nanf128 (void)
{
  return __builtin_nanf128 ("");
}

__float128
my_nansq1 (void)
{
  return __builtin_nansq ("");
}

_Float128
my_nansq2 (void)
{
  return __builtin_nansq ("");
}

_Float128
my_nansf128 (void)
{
  return __builtin_nansf128 ("");
}
