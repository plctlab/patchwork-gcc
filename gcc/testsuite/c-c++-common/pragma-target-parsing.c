/* Test that we provide good warnings for malformed '#pragma target'.  */

#pragma GCC target /* { dg-warning "ignoring malformed '#pragma GCC target': expected a string option" } */
#pragma GCC target 1066 /* { dg-warning "ignoring malformed '#pragma GCC target': expected a string option" } */
#pragma GCC target( /* { dg-warning "ignoring malformed '#pragma GCC target': expected a string option" } */
#pragma GCC target() /* { dg-warning "ignoring malformed '#pragma GCC target': expected a string option" } */
#pragma GCC target("", )
#pragma GCC target("",  /* { dg-warning "'#pragma GCC target \\(string \\\[,string\\\]...\\)' does not have a final '\\)'" } */

void foo ()
{
#pragma GCC target /* { dg-error "#pragma GCC target option' is not allowed inside functions" } */
  /* { dg-warning "ignoring malformed '#pragma GCC target': expected a string option" "" { target c++ } .-1 } */
}
