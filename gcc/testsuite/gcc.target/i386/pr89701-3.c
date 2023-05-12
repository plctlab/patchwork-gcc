/* { dg-do compile { target *-*-linux* } } */
/* { dg-options "-fcf-protection=return,none" } */
/* { dg-warning "combination of '-fcf-protection=' option: none is ignored" "" { target { *-*-linux* } } 0 } */
/* { dg-final { scan-assembler-times ".note.gnu.property" 1 } } */
/* { dg-final { scan-assembler-times ".long	0x2" 1 } } */
