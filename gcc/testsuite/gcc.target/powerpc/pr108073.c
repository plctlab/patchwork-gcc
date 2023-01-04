/* { dg-do run } */
/* { dg-options "-O2 -save-temps" } */

typedef struct DF {double a[4]; short s1; short s2; short s3; short s4; } DF;
typedef struct SF {float a[4]; int i1; int i2; } SF;

/* Each of below function contains one mtvsrd.  */
/* { dg-final { scan-assembler-times {\mmtvsrd\M} 3 {target { has_arch_ppc64 && has_arch_pwr8 } } } } */
/* { dg-final { scan-assembler-not {\mlwz\M} {target { has_arch_ppc64 && has_arch_pwr8 } } } } */
/* { dg-final { scan-assembler-not {\mlhz\M} {target { has_arch_ppc64 && has_arch_pwr8 } } } } */
short  __attribute__ ((noipa)) foo_hi (DF a, int flag){if (flag == 2)return a.s2+a.s3;return 0;}
int  __attribute__ ((noipa)) foo_si (SF a, int flag){if (flag == 2)return a.i2+a.i1;return 0;}
double __attribute__ ((noipa)) foo_df (DF arg, int flag){if (flag == 2)return arg.a[3];else return 0.0;}
float  __attribute__ ((noipa)) foo_sf (SF arg, int flag){if (flag == 2)return arg.a[2]; return 0;}
float  __attribute__ ((noipa)) foo_sf1 (SF arg, int flag){if (flag == 2)return arg.a[1];return 0;}

DF gdf = {{1.0,2.0,3.0,4.0}, 1, 2, 3, 4};
SF gsf = {{1.0f,2.0f,3.0f,4.0f}, 1, 2};

int main()
{
  if (!(foo_hi (gdf, 2) == 5 && foo_si (gsf, 2) == 3 && foo_df (gdf, 2) == 4.0
	&& foo_sf (gsf, 2) == 3.0 && foo_sf1 (gsf, 2) == 2.0))
    __builtin_abort ();
  if (!(foo_hi (gdf, 1) == 0 && foo_si (gsf, 1) == 0 && foo_df (gdf, 1) == 0
	&& foo_sf (gsf, 1) == 0 && foo_sf1 (gsf, 1) == 0))
    __builtin_abort ();
  return 0;
}

