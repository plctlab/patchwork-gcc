/* { dg-do compile } */
/* { dg-options "-march=rv64gvcw_zvl128_s123_x123 -mabi=lp64d" } */
int foo()
{
}

/* { dg-error "ISA string is not in canonical order. 'c'" "" { target *-*-* } 0 } */
/* { dg-error "extension 'w' is unsupported standard single letter extension" "" { target *-*-* } 0 } */
/* { dg-error "extension 'zvl128' start with `z` but is unsupported standard extension" "" { target *-*-* } 0 } */
/* { dg-error "extension 's123' start with `s` but is unsupported standard supervisor extension" "" { target *-*-* } 0 } */
/* { dg-error "extension 'x123' start with `x` but is unsupported non-standard extension" "" { target *-*-* } 0 } */
