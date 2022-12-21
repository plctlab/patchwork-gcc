/* { dg-do run } */
/* { dg-options "-O2 -save-temps" } */

typedef struct DF {double a[4]; long l; } DF;
typedef struct SF {float a[4];short l; } SF;

/* Each of below function contains one mtvsrd.  */
/* { dg-final { scan-assembler-times {\mmtvsrd\M} 3 {target { has_arch_ppc64 && has_arch_pwr8 } } } } */
double __attribute__ ((noipa)) foo_df (DF arg){return arg.a[3];}
float  __attribute__ ((noipa)) foo_sf (SF arg){return arg.a[2];}
float  __attribute__ ((noipa)) foo_sf1 (SF arg){return arg.a[1];}

double gd = 4.0;
float gf1 = 3.0f, gf2 = 2.0f;
DF gdf = {{1.0,2.0,3.0,4.0}, 1L};
SF gsf = {{1.0f,2.0f,3.0f,4.0f}, 1L};

int main()
{
  if (!(foo_df (gdf) == gd && foo_sf (gsf) == gf1 && foo_sf1 (gsf) == gf2))
    __builtin_abort ();
  return 0;
}

