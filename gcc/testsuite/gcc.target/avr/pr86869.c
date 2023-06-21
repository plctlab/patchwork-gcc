/* { dg-do compile } */
/* { dg-options "-std=gnu99" } */

extern void bar(const __memx void* p);

struct S {
  char y[2];
};
extern const __memx struct S *s;

void foo(void) {
  bar(&s->y);
}
