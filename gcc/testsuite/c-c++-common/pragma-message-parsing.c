/* Test that we provide good warnings for malformed '#pragma message'.  */

#pragma message  /* { dg-line no_arg } */
/* { dg-warning "expected a string after '#pragma message'" "" { target *-*-* } no_arg } */

#pragma message 1066  /* { dg-line bad_arg } */
/* { dg-warning "17: expected a string after '#pragma message'" "" { target *-*-* } bad_arg } */

#pragma message "foo" /* { dg-message "#pragma message: foo" } */

#pragma message(1066)  /* { dg-line parens_bad_arg } */
/* { dg-warning "17: expected a string after '#pragma message'" "" { target *-*-* } parens_bad_arg } */

#pragma message("bar") /* { dg-message "#pragma message: bar" } */

#pragma message("foo") bar  /* { dg-warning "24: junk at end of '#pragma message'" } */

#if defined(__cplusplus) && __cplusplus >= 201103L
unsigned operator ""_w(const char*);
#pragma message "foo"_w /* { dg-message "17: string literal with user-defined suffix is invalid in this context" "" { target { c++11 } } } */
#endif
