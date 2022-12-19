/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-g" "-Oz" "-Os"} } */
/* { dg-options "-march=rv64gc_xtheadmempair -mtune=thead-c906" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_xtheadmempair -mtune=thead-c906" { target { rv32 } } } */

int x[4] = {-4,-5,6,7};
int y[4];

unsigned int*
foo()
{
	y[0] = (unsigned int) x[0];
	y[2] = (unsigned int) x[1];
	y[1] = (unsigned int) x[2];
	y[3] = (unsigned int) x[3];
	return y;
}

/* { dg-final { scan-assembler-times "th.lwud\t" 2 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.swd\t" 2 { target { rv32 } } } } */
/* { dg-final { scan-assembler-times "th.lwd\t" 2 { target { rv32 } } } } */
