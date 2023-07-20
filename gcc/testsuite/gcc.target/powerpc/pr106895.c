/* PR target/106895 */
/* { dg-require-effective-target int128 } */
/* { dg-options "-O2" } */

/* Verify the following generates even/odd register pairs.  */

typedef __int128 pti __attribute__((mode(PTI)));

void
set128 (pti val, pti *mem)
{
    asm("stq %1,%0" : "=m"(*mem) : "r"(val));
}

/* { dg-final { scan-assembler "stq 10,0\\(5\\)" } } */
