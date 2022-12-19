/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-g" "-Oz" "-Os"} } */
/* { dg-options "-march=rv64gc_xtheadmempair -mtune=thead-c906" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_xtheadmempair -mtune=thead-c906" { target { rv32 } } } */

unsigned int arr[4][4] = {{0, 1, 1, 2}, {2, 2, 1, 2}, {1, 2, 1, 1}, {1, 2, 2, 0}};

unsigned long long
foo ()
{
  unsigned long long ll;
  ll = arr[1][0];
  ll += arr[1][1];
  return ll;
}

/* { dg-final { scan-assembler-times "th.lwud\t" 1 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.lwd\t" 1 { target { rv32 } } } } */


