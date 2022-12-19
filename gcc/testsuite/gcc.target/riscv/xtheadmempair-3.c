/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-g" "-Oz" "-Os"} } */
/* { dg-options "-march=rv64gc_xtheadmempair -mtune=thead-c906 --param=sched-autopref-queue-depth=10" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_xtheadmempair -mtune=thead-c906 --param=sched-autopref-queue-depth=10" { target { rv32 } } } */

int arr[4][4];

void
foo (int x, int y)
{
	arr[0][1] = x;
	arr[1][0] = y;
	arr[2][0] = x;
	arr[1][1] = y;
	arr[0][2] = x;
	arr[0][3] = y;
	arr[1][2] = x;
	arr[2][1] = y;
	arr[3][0] = x;
	arr[3][1] = y;
	arr[2][2] = x;
	arr[1][3] = y;
	arr[2][3] = x;
	arr[3][2] = y;
}

/* { dg-final { scan-assembler-times "addi\t" 5 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.swd\t" 7 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "addi\t" 5 { target { rv32 } } } } */
/* { dg-final { scan-assembler-times "th.swd\t" 7 { target { rv32 } } } } */
