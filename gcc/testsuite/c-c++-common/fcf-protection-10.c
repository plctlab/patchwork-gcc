/* { dg-do compile { target { "i?86-*-* x86_64-*-*" } } } */
/* { dg-options "-fcf-protection=branch,check" } */
/* { dg-error "Combination for '-fcf-protection=' option: check is not valid" "" { target *-*-* } 0 } */
