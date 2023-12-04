// { dg-do run { target c++23 } }

#include <memory>
#include <testsuite_hooks.h>

// C++23 [inout.ptr.t] Class template inout_ptr_t
// Verify that implementation handles LWG Issue 3897
void nuller(int **p) {
  *p = nullptr;
}

int main(int, char **) {
  int *i = new int{5};
  nuller(std::inout_ptr(i));

  VERIFY(i == nullptr);
}
