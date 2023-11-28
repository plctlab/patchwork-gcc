// PR c++/112658
// { dg-do compile { target c++11 } }

void f(int*);

int main() {
  using array = int[];
  f(array{42}); // { dg-error "address of temporary array" }
  f((int*)array{42}); // { dg-error "address of temporary array" }
}
