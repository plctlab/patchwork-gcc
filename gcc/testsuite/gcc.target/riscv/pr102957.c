/* { dg-do compile } */
/* { dg-options "-march=rv64gzb -mabi=lp64" } */
int foo()
{
}

/* { dg-error "extension 'zb' starts with `z` but is not a standard sub-extension" "" { target *-*-* } 0 } */
