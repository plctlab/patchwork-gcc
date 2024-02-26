/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler-not {\mst[hb]\M} } } */

/* Verify that memclear takes overlap store. */
void* foo (char* s1)
{
  __builtin_memset (s1, 0, 31);
}
