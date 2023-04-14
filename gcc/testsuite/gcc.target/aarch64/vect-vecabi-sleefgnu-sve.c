/* { dg-do compile } */
/* { dg-options "-O3 -march=armv8-a+sve -ftree-vectorize -mveclibabi=sleefgnu -ffast-math" } */

extern float sinf(float);

float x[256];

void foo(void)
{
  int i;

  for (i=0; i<256; ++i)
    x[i] = sinf(x[i]);
}

/* { dg-final { scan-assembler "_ZGVsNxv_sinf" } } */
