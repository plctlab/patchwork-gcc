// P0847R7
// { dg-do run { target c++23 } }

// test implicit conversion of the object argument
// to the explicit object parameter

// we compare &s to ret because early on, the
// object parameter would not convert, it would just get
// reinterpreted as the type of the explicit object param

// { dg-xfail-run-if "user defined conversions from an implicit object argument to an explicit object parameter are not supported yet" { *-*-* } }

using uintptr_t = __UINTPTR_TYPE__;

struct S {
    operator uintptr_t() const {
	return 42;
    }
    uintptr_t f(this uintptr_t n) {
        return n;
    }
};

int main() 
{
  S s{};
  uintptr_t ret = s.f();
  if (ret == reinterpret_cast<uintptr_t>(&s))
    __builtin_abort ();
  if (ret != 42)
    __builtin_abort ();
}

