/* Verify that we use precise locations when emitting diagnostics
   about pragmas.  */

/* { dg-do assemble } */
/* { dg-options "-fdiagnostics-show-caret" } */

/* pack ****************************************************************************/

#pragma pack
/* { dg-warning "ignoring malformed '#pragma pack': expected '\\('" "" { target *-*-* } .-1 }
   { dg-begin-multiline-output "" }
 #pragma pack
             ^
   { dg-end-multiline-output "" }  */

#pragma pack (
/* { dg-warning "ignoring malformed '#pragma pack': expected '\\)', integer, 'push', or 'pop'" "" { target *-*-* } .-1 }
   { dg-begin-multiline-output "" }
 #pragma pack (
               ^
   { dg-end-multiline-output "" }  */

#pragma pack (32
/* { dg-warning "ignoring malformed '#pragma pack': expected '\\)'" "" { target *-*-* } .-1 }
   { dg-begin-multiline-output "" }
 #pragma pack (32
                 ^
   { dg-end-multiline-output "" }  */

#pragma pack (3.14159
/* { dg-warning "ignoring malformed '#pragma pack': invalid constant" "" { target *-*-* } .-1 }
   { dg-begin-multiline-output "" }
 #pragma pack (3.14159
               ^~~~~~~
   { dg-end-multiline-output "" }  */

#pragma pack (push, 3.14159
/* { dg-warning "ignoring malformed '#pragma pack': invalid constant" "" { target *-*-* } .-1 }
   { dg-begin-multiline-output "" }
 #pragma pack (push, 3.14159
                     ^~~~~~~
   { dg-end-multiline-output "" }  */

#pragma pack (toothbrush
/* { dg-warning "ignoring malformed '#pragma pack': unknown action 'toothbrush'" "" { target *-*-* } .-1 }
   { dg-begin-multiline-output "" }
 #pragma pack (toothbrush
               ^~~~~~~~~~
   { dg-end-multiline-output "" }  */

#pragma pack() pyjamas
/* { dg-warning "junk at end of '#pragma pack'" "" { target *-*-* } .-1 }
   { dg-begin-multiline-output "" }
 #pragma pack() pyjamas
                ^~~~~~~
   { dg-end-multiline-output "" }  */

/* target ****************************************************************************/

#pragma GCC target 42
/* { dg-warning "ignoring malformed '#pragma GCC target': expected a string option" "" { target *-*-* } .-1 }
   { dg-begin-multiline-output "" }
 #pragma GCC target 42
                    ^~
   { dg-end-multiline-output "" }  */

#pragma GCC target ( 1776
/* { dg-warning "ignoring malformed '#pragma GCC target': expected a string option" "" { target *-*-* } .-1 }
   { dg-begin-multiline-output "" }
 #pragma GCC target ( 1776
                      ^~~~
   { dg-end-multiline-output "" }  */

/* message ****************************************************************************/

#pragma message "foo" int
/* { dg-warning "junk at end of '#pragma message'" "" { target *-*-* } .-1 }
   { dg-message "'#pragma message: foo'" "" { target *-*-* } .-2 }
   { dg-begin-multiline-output "" }
 #pragma message "foo" int
                       ^~~
   { dg-end-multiline-output "" }
   { dg-begin-multiline-output "" }
 #pragma message "foo" int
         ^~~~~~~
   { dg-end-multiline-output "" }  */
