/* Test virtual location aspects of _Pragmas, when an error is reported after
   lexing the tokens from the _Pragma string.  */
/* { dg-additional-options "-Wpragmas -Wunknown-pragmas" } */

_Pragma("GCC diagnostic ignored \"oops1\"") /* { dg-note {1:in <_Pragma directive>} } */
/* { dg-warning {24:'oops1' is not an option} "" { target *-*-* } 1 } */

#define S2 "GCC diagnostic ignored \"oops2\""
_Pragma(S2) /* { dg-note {1:in <_Pragma directive>} } */
/* { dg-warning {24:'oops2' is not an option} "" { target *-*-* } 1 } */

#define PP(x) _Pragma(x) /* { dg-note {15:in <_Pragma directive>} } */
PP("GCC diagnostic ignored \"oops3\"") /* { dg-note {1:in expansion of macro 'PP'} } */
/* { dg-warning {24:'oops3' is not an option} "" { target *-*-* } 1 } */

#define X4 _Pragma("GCC diagnostic ignored \"oops4\"") /* { dg-note {12:in <_Pragma directive>} } */
#define Y4 X4 /* { dg-note {12:in expansion of macro 'X4'} } */
Y4 /* { dg-note {1:in expansion of macro 'Y4'} } */
/* { dg-warning {24:'oops4' is not an option} "" { target *-*-* } 1 } */

#define P5 _Pragma /* { dg-note {12:in <_Pragma directive>} } */
#define S5 "GCC diagnostic ignored \"oops5\""
#define Y5 P5(S5) /* { dg-note {12:in expansion of macro 'P5'} } */
Y5 /* { dg-note {1:in expansion of macro 'Y5'} } */
/* { dg-warning {24:'oops5' is not an option} "" { target *-*-* } 1 } */

#define P6 _Pragma /* { dg-note {12:in <_Pragma directive>} } */
#define X6 P6("GCC diagnostic ignored \"oops6\"") /* { dg-note {12:in expansion of macro 'P6'} } */
X6 /* { dg-note {1:in expansion of macro 'X6'} } */
/* { dg-warning {24:'oops6' is not an option} "" { target *-*-* } 1 } */

_Pragma(__DATE__) /* { dg-warning {-:[-Wunknown-pragmas]} } */

_Pragma("once") /* { dg-note {1:in <_Pragma directive>} } */
/* { dg-warning {#pragma once in main file} "" { target *-*-*} 1 } */
