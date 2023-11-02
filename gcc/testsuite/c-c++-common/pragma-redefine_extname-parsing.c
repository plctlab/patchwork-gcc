/* Test that we provide good warnings for malformed '#pragma redefine_extname'.  */

/* Missing old name.  */
#pragma redefine_extname /* { dg-warning "ignoring malformed '#pragma redefine_extname': expected symbol name" } */
#pragma redefine_extname 42 /* { dg-warning "26: ignoring malformed '#pragma redefine_extname': expected symbol name" } */

/* Missing new name.  */
#pragma redefine_extname foo /* { dg-warning "ignoring malformed '#pragma redefine_extname': expected symbol name" } */
#pragma redefine_extname foo 42 /* { dg-warning "30: ignoring malformed '#pragma redefine_extname': expected symbol name" } */
