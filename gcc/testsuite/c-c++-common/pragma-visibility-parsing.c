/* Test that we provide good warnings for malformed '#pragma GCC visibility'.  */

#pragma GCC visibility /* { dg-line no_action } */
/* { dg-warning "'#pragma GCC visibility' must be followed by 'push' or 'pop'" "" { target *-*-* } no_action } */

#pragma GCC visibility push 1066  /* { dg-warning "29: ignoring malformed '#pragma GCC visibility': expected '\\('" } */

#pragma GCC visibility push(foo) /* { dg-line bad_push_arg } */
/* { dg-warning "29: '#pragma GCC visibility push\\(\\)' must specify 'default', 'internal', 'hidden' or 'protected'" "" { target *-*-* } bad_push_arg }
   { dg-message "ignoring malformed '#pragma GCC visibility'" "" { target *-*-* } bad_push_arg } */

#pragma GCC visibility push(default)
#pragma GCC visibility pop 1066  /* { dg-warning "28: junk at end of '#pragma GCC visibility'" } */
