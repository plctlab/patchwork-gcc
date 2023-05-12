/* { dg-do compile { target *-*-linux* } } */
/* { dg-options "-fcf-protection=branch,none" } */
/* { dg-warning "combination of '-fcf-protection=' option: none is ignored" "" { target { *-*-* } } 0 } */
/* { dg-final { scan-assembler-times ".note.gnu.property" 1 } } */
/* { dg-final { scan-assembler-times ".long	0x1" 1 } } */
