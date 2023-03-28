/* { dg-do compile } */
/* { dg-options "-march=rv64if_zfinx -mabi=lp64" } */
int foo() {}
/* { dg-error "z*inx is conflict with float extensions" "" { target *-*-* } 0 } */
