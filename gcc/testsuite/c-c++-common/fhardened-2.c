/* { dg-do compile } */
/* { dg-options "-fhardened -fstack-protector -fno-PIE" } */

#ifdef __SSP_STRONG__
# error "-fstack-protector-strong enabled when it should not be"
#endif
#ifndef __SSP__
# error "-fstack-protector not enabled"
#endif
#ifdef __PIE__
# error "PIE enabled when it should not be"
#endif
