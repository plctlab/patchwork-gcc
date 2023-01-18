/* { dg-options "-O2" } */
/* { dg-require-effective-target int128 } */

unsigned __int128
and128WithConst1 (unsigned __int128 a)
{
   unsigned __int128 c128 = (((unsigned __int128)(~0ULL)) << 64)
			    | ((unsigned __int128)(~0xFULL));
   return a & c128;
}

unsigned __int128
and128WithConst2 (unsigned __int128 a)
{
   unsigned __int128 c128 = (((unsigned __int128)(~0ULL)) << 64)
			    | ((unsigned __int128)(0xFFULL));
   return a & c128;
}

unsigned __int128
and128WithConst3 (unsigned __int128 a)
{
   unsigned __int128 c128 = (((unsigned __int128)(~0ULL)) << 64)
			    | ((unsigned __int128)(0xF1ULL));
   return a & c128;
}

unsigned __int128
and128WithConst4 (unsigned __int128 a)
{
   unsigned __int128 c128 = (((unsigned __int128) (~0xFULL)) << 64)
			    | ((unsigned __int128) (~0ULL));
   return a & c128;
}

unsigned __int128
and128WithConst5 (unsigned __int128 a)
{
   unsigned __int128 c128 = (((unsigned __int128) (0xF1ULL)) << 64)
			    | ((unsigned __int128) (~0ULL));
   return a & c128;
}

/* { dg-final { scan-assembler-times {\mrldicr\M} 2 } } */
/* { dg-final { scan-assembler-times {\mrldicl\M} 1 } } */
/* { dg-final { scan-assembler-times {\mandi\M} 2 } } */
