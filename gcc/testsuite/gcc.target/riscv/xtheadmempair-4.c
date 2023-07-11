/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-g" "-Oz" "-Os" "-flto" } } */
/* { dg-options "-march=rv64gc_xtheadmempair -O2 -g -mtune=thead-c906" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_xtheadmempair -O2 -g -mtune=thead-c906" { target { rv32 } } } */

void a();
void b(char *);
void m_fn1(int);
int e;

int foo(int ee, int f, int g) {
  char *h = (char *)__builtin_alloca(1);
  b(h);
  b("");
  int i = ee;
  e = g;
  m_fn1(f);
  a();
  e = i;
}

/* { dg-final { scan-assembler-times "th.ldd\t" 3 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.sdd\t" 3 { target { rv64 } } } } */

/* { dg-final { scan-assembler-times "th.lwd\t" 3 { target { rv32 } } } } */
/* { dg-final { scan-assembler-times "th.swd\t" 3 { target { rv32 } } } } */
