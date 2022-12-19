/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-g" "-Oz" "-Os"} } */
/* { dg-options "-march=rv64gc_xtheadmempair -mtune=thead-c906" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_xtheadmempair -mtune=thead-c906" { target { rv32 } } } */

void
store (int *arr, int x, int y, int z)
{
  arr[400] = x;
  arr[401] = y;

  arr[500] = z;
  arr[501] = x;
}

void
store_long (long long int *arr, long long int x, long long int y)
{
  arr[400] = x;
  arr[401] = y;

  arr[403] = y;
  arr[404] = x;
}

/* { dg-final { scan-assembler-times "addi\t" 4 } } */
/* { dg-final { scan-assembler-times "th.sdd\t" 2 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.swd\t" 2 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.swd\t" 6 { target { rv32 } } } } */

