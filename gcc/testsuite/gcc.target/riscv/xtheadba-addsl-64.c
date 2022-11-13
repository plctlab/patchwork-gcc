/* { dg-do compile } */
/* { dg-options "-march=rv64gc_xtheadba -mabi=lp64" } */
/* { dg-skip-if "" { *-*-* } { "-O0" } } */

/* RV64 only.  */
int foos(short *x, int n){
  return x[n];
}
int fooi(int *x, int n){
  return x[n];
}
int fooll(long long *x, int n){
  return x[n];
}

/* { dg-final { scan-assembler-times "th.addsl\[ \t\]*a\[0-9\]+,a\[0-9\]+,a\[0-9\]+,1" 1 } } */
/* { dg-final { scan-assembler-times "th.addsl\[ \t\]*a\[0-9\]+,a\[0-9\]+,a\[0-9\]+,2" 1 } } */
/* { dg-final { scan-assembler-times "th.addsl\[ \t\]*a\[0-9\]+,a\[0-9\]+,a\[0-9\]+,3" 1 } } */
