/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-g" "-Oz" "-Os"} } */
/* { dg-options "-march=rv64gc_xtheadmempair -mtune=thead-c906" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_xtheadmempair -mtune=thead-c906" { target { rv32 } } } */

void
store_offset (int *array, int x, int y)
{
  array[1085] = x;
  array[1084] = y;

  array[1086] = y;
  array[1087] = x;
}

/* { dg-final { scan-assembler-times "addi\t" 1 } } */
/* { dg-final { scan-assembler-times "th.swd\t" 2 } } */

