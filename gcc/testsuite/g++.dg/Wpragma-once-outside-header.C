/* { dg-do assemble  } */
/* { dg-options "-Werror=pragma-once-outside-header" } */

#pragma once  // { dg-error "#pragma once in main file" }
int main() {}
