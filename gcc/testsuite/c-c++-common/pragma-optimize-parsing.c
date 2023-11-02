/* Test that we provide good warnings for malformed '#pragma GCC optimize'.  */

#pragma GCC optimize /* { dg-warning "'#pragma GCC optimize' is not a string or number" } */
#pragma GCC optimize 3
#pragma GCC optimize(  /* { dg-warning "'#pragma GCC optimize' is not a string or number" } */
#pragma GCC optimize() /* { dg-warning "'#pragma GCC optimize' is not a string or number" } */
#pragma GCC optimize(3, )
#pragma GCC optimize("-O3", )
#pragma GCC optimize(3, /* { dg-warning "'#pragma GCC optimize \\(string \\\[,string\\\]...\\)' does not have a final '\\)'" } */

void foo ()
{
#pragma GCC optimize /* { dg-error "#pragma GCC optimize' is not allowed inside functions" } */ 
}

#pragma GCC optimize("-Ofan-noise", ) /* { dg-error "argument to '-O' should be a non-negative integer, 'g', 's', 'z' or 'fast'" } */
