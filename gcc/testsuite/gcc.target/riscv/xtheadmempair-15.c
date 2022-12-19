/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-g" "-Oz" "-Os"} } */
/* { dg-options "-march=rv64gc_xtheadmempair -mtune=thead-c906" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_xtheadmempair -mtune=thead-c906" { target { rv32 } } } */

long long
load_long (long long int *arr)
{
  return arr[400] << 1 + arr[401] << 1 + arr[403] << 1 + arr[404] << 1;
}

/* { dg-final { scan-assembler-times "addi\t" 1 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.ldd\t" 1 { target { rv64 } } } } */

/* { dg-final { scan-assembler-times "th.lwd\t" 1 { target { rv32 } } } } */
