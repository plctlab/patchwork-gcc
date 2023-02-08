/* { dg-do compile } */
/* { dg-require-effective-target int128 } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler-times {(?n)^\s+[a-z]} 3 } } */

/* It just needs two std and one blr.  */
void foo (unsigned __int128* res, unsigned long long hi, unsigned long long lo)
{
   unsigned __int128 i = hi;
   i <<= 64;
   i |= lo;
   *res = i;
}

