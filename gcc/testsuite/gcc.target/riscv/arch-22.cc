/* { dg-do compile } */
/* { dg-options "-march=rv64gcv_zvl128_z123_s123 -mabi=lp64d" } */
int foo()
{
}
/* { dg-error "extension 'zvl128' start with `z` but not a standard sub-extension" "" { target *-*-* } 0 } */
/* { dg-error "extension 'z123' start with `z` but not a standard sub-extension" "" { target *-*-* } 0 } */
/* { dg-error "extension 's123' start with `s` but not a standard supervisor extension" "" { target *-*-* } 0 } */
