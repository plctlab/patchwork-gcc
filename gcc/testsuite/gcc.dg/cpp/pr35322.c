/* Test case for PR 35322 -- _Pragma ICE.  */

/* { dg-do preprocess } */
/* { dg-additional-options "-ftrack-macro-expansion=0" } */
_Pragma("GCC dependency") /* { dg-error "#pragma dependency expects" } */
