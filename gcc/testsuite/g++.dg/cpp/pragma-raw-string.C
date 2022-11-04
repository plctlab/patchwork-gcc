/* Test that _Pragma with a raw string works correctly.  */
/* { dg-do compile { target c++11 } } */
/* { dg-additional-options "-Wunused-variable -Wpragmas" } */

_Pragma(R"delim(GCC diagnostic push)delim")
_Pragma(R"(GCC diagnostic ignored "-Wunused-variable")")
void f1 () { int i; }
_Pragma(R"(GCC diagnostic pop)")
void f2 () { int i; } /* { dg-warning {18:-Wunused-variable} } */

/* Make sure lines stay in sync if there is an embedded newline too.  */
_Pragma(R"xyz(GCC diagnostic ignored R"(two
line option?)")xyz")
/* { dg-note {1:in <_Pragma directive>} "" { target *-*-* } .-2 } */
/* { dg-warning {24:unknown option} "" { target *-*-* } 1 } */
void f3 () { int i; } /* { dg-warning {18:-Wunused-variable} } */
