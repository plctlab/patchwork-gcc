/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-g" "-Oz" "-Os"} } */
/* { dg-options "-march=rv64gc_xtheadmempair -mtune=thead-c906" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_xtheadmempair -mtune=thead-c906" { target { rv32 } } } */

int x[4] = {-4,-5,6,7};

int
foo()
{
  int a,b,c;
  
  a = x[0];
  b = x[1];
  c = x[2];
  return a+b+c;
}

/* { dg-final { scan-assembler-times "th.lwd\t" 1 } } */
