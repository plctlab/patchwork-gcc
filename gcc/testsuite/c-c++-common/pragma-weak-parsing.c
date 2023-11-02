/* Test that we provide good warnings for malformed '#pragma weak'.  */

int bar;

#pragma weak  /* { dg-warning "ignoring malformed '#pragma weak': expected name" } */

#pragma weak 42  /* { dg-warning "14: ignoring malformed '#pragma weak': expected name" } */

/* This should be OK.  */
#pragma weak foo

#pragma weak foo 1066  /* { dg-warning "18: junk at end of '#pragma weak'" } */

#pragma weak foo =  /* { dg-warning "ignoring malformed '#pragma weak': expected name" } */

#pragma weak foo = 1066  /* { dg-warning "20: ignoring malformed '#pragma weak': expected name" } */

/* This should be OK.  */
#pragma weak foo = bar

#pragma weak foo = bar baz /* { dg-warning "24: junk at end of '#pragma weak'" } */

typedef int foo_t; /* { dg-message "13: 'foo_t' is not a variable or function" } */
#pragma weak foo_t /* { dg-warning "14: '#pragma weak' declaration of 'foo_t' not allowed, ignored" } */
