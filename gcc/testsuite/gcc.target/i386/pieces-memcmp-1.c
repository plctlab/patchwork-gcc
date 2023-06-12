/* { dg-do compile } */
/* { dg-options "-O2" } */

int foo(char *a)
{
    static const char t[] = "0123456789012345678901234567890";
    return __builtin_memcmp(a, &t[0], sizeof(t)) == 0;
}

/* { dg-final { scan-assembler-not "xorl\[ \\t]*\\\$1," } } */
