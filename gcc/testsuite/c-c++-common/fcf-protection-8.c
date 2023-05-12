/* { dg-do compile { target { "i?86-*-* x86_64-*-*" } } } */
/* { dg-options "-fcf-protection=branch,none" } */
/* { dg-warning "combination of '-fcf-protection=' option: none is ignored" "" { target { *-*-* } } 0 } */
