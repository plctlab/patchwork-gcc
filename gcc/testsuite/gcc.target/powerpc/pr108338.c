// { dg-do run }
// { dg-options "-O2 -save-temps" }

float __attribute__ ((noipa)) sf_from_di_off0 (long long l)
{
  char buff[16];
  *(long long*)buff = l;
  float f = *(float*)(buff);
  return f;    
}

float  __attribute__ ((noipa)) sf_from_di_off4 (long long l)
{
  char buff[16];
  *(long long*)buff = l;
  float f = *(float*)(buff + 4);
  return f; 
}

/* Under lp64, 'l' is in one DI reg, then check sub DI to SF. */
/* { dg-final { scan-assembler-times {\mrldicr\M} 1 { target { lp64 && has_arch_pwr8 } } } } */
/* { dg-final { scan-assembler-times {\mxscvspdpn\M} 2 { target { lp64 && has_arch_pwr8 } } } } */

/* { dg-final { scan-assembler-times {\mmtvsrd\M} 2 { target { lp64 && { has_arch_pwr8 && { ! has_arch_pwr9 } } } } } } */
/* { dg-final { scan-assembler-times {\mmtvsrd\M} 1 { target { lp64 && has_arch_pwr9 } } } } */
/* { dg-final { scan-assembler-times {\mmtvsrws\M} 1 { target { lp64 && has_arch_pwr9 } } } } */

union di_sf_sf
{
  struct {float f1; float f2;};
  long long l;
};

int main()
{
  union di_sf_sf v;
  v.f1 = 1.0f;
  v.f2 = 2.0f;
  if (sf_from_di_off0 (v.l) != 1.0f || sf_from_di_off4 (v.l) != 2.0f )
    __builtin_abort ();
  return 0;
}
