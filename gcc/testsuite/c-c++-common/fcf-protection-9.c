/* { dg-do compile { target { "i?86-*-* x86_64-*-*" } } } */
/* { dg-options "-fcf-protection=branch,full" } */
/* { dg-warning "better to use '-fcf-protection=' option: full alone instead of in a combination" "" { target { *-*-* } } 0 } */
