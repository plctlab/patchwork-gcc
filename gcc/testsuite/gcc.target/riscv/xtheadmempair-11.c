/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-g" "-Oz" "-Os"} } */
/* { dg-options "-march=rv64gc_xtheadmempair -mtune=thead-c906" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_xtheadmempair -mtune=thead-c906" { target { rv32 } } } */

int arr[4][4] = {{0, 1, 1, -1}, {-1, -1, 1, -1}, {1, -1, 1, 1}, {1, -1, -1, 0}};

long long
foo ()
{
  long long ll;
  ll = arr[1][0];
  ll += arr[1][1];
  ll += arr[1][2];
  return ll;
}

/* { dg-final { scan-assembler-times "th.lwd\t" 1 } } */
