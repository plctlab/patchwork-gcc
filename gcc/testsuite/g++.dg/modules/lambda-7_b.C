// { dg-module-do run }
// { dg-additional-options "-fmodules-ts" }

import foo;

int main() {
  S s;
  if (s.a(10) != 20)
    __builtin_abort();
  if (s.b(10) != 30)
    __builtin_abort();
  if (s.c(10) != 40)
    __builtin_abort();
  if (d(10) != 50)
    __builtin_abort();
}
