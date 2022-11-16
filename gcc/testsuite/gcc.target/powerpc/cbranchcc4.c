/* { dg-do compile } */
/* { dg-options "-O2 -fdump-rtl-ce1" } */
/* { dg-final {scan-rtl-dump "noce_try_store_flag_constants" "ce1" } } */

int test (unsigned int a, unsigned int b)
{
    return (a < b ? 0 : (a > b ? 2 : 1));
}
