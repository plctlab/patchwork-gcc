/* Test virtual location aspects of _Pragmas, when an error is reported during
   lexing of the _Pragma string itself or of the tokens within it.  */
/* { dg-additional-options "-Wpragmas" } */

#define X1 "\""
_Pragma(X1) /* { dg-note {1:in <_Pragma directive>} } */
/* { dg-warning {1:missing terminating " character} "" { target *-*-* } 1 } */

#define X2a _Pragma("GCC warning \"hello\"") ( /* { dg-note {13:in <_Pragma directive>} } */
#define X2b "GCC warning \"goodbye\"" )
_Pragma X2a X2b /* { dg-note {9:in expansion of macro 'X2a'} } */
/* { dg-note {1:in <_Pragma directive>} "" { target *-*-* } .-1 } */
/* { dg-warning {13:hello} "" { target *-*-* } 1 } */
/* { dg-warning {13:goodbye} "" { target *-*-* } 1 } */

_Pragma() /* { dg-error {9:_Pragma takes a parenthesized string literal} } */
/* { dg-note {1:in <_Pragma directive>} "" { target *-*-* } .-1 } */
/* { dg-error {at end of input|'_Pragma' does not name a type} "" { target *-*-* } .-2 } */
