/* { dg-do compile { target { powerpc*-*-aix* powerpc*-*-linux* } } } */
/* { dg-require-effective-target fpic } */
/* { dg-options "-fpic -mno-pcrel -O2 -msave-toc-indirect" } */

/* Verify the explicit -msave-toc-indirect always
   takes effect, so there is only one toc saving.  */

void foo (void) __attribute__((__longcall__));
int baz (void) __attribute__((__longcall__));

__attribute__ ((cold)) int
bar (void)
{
  foo ();
  return baz () + 1;
}

/* Linux LE */
/* { dg-final { scan-assembler-times {\mstd 2,24\(1\)} 1 { target powerpc_elfv2 } } } */
/* Linux BE 64 bit or AIX 64 bit */
/* { dg-final { scan-assembler-times {\mstd 2,40\(1\)} 1 { target { lp64 && { ! powerpc_elfv2 } } } } } */
/* AIX 32 bit */
/* { dg-final { scan-assembler-times {\mstw 2,20\(1\)} 1 { target { ilp32 && powerpc*-*-aix* } } } } */

