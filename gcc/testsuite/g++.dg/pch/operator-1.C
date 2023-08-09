/* { dg-additional-options "-ftrack-macro-expansion=0" } */
#include "operator-1.H"
int main(void){ major(0);} /* { dg-warning "Did not Work" } */
