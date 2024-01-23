/* { dg-do compile } */
/* { dg-options "-march=armv8.1-m.main+pacbti -O1 -mbranch-protection=bti --save-temps" } */

#include <stdio.h>

struct C18 {
  virtual void f7();
};

struct C19 : virtual C18 {
  virtual void f7();
};

void C19::f7() {
  printf("foo\n");
}

/* { dg-final { scan-assembler-times "\tbti" 2 } } */