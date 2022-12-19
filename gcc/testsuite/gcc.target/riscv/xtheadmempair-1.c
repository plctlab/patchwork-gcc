/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-g" "-Oz" "-Os" "-funroll-loops"} } */
/* { dg-options "-march=rv64gc_xtheadmempair -mtune=thead-c906" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_xtheadmempair -mtune=thead-c906" { target { rv32 } } } */

#define A_SIZE 32

unsigned long long array[A_SIZE];
unsigned long long temp[A_SIZE];

void bar (void);

int
foo (void)
{
  for (int i=0; i<A_SIZE; i++)
    array[i] = 0xF;
  
  bar();

  for (int i=0; i<A_SIZE; i++)
    temp[i] = array[i];

  return 1;
}
/* { dg-final { scan-assembler-times "th.ldd\t" 3 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.sdd\t" 3 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.lwd\t" 3 { target { rv32 } } } } */
/* { dg-final { scan-assembler-times "th.swd\t" 4 { target { rv32 } } } } */
