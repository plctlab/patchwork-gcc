/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-g" "-Oz" "-Os"} } */
/* { dg-options "-march=rv64gc_xtheadmempair -mtune=thead-c906" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_xtheadmempair -mtune=thead-c906" { target { rv32 } } } */

int
load (int *arr)
{
  return (arr[400] + arr[401] + arr[527] + arr[528]);
}

/* { dg-final { scan-assembler-times "addi\t" 2 } } */
/* { dg-final { scan-assembler-times "th.lwd\t" 2 } } */
