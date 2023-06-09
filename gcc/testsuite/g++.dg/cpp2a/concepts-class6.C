// PR c++/109655
// { dg-do compile { target c++20 } }

class C {
  template<class>
  requires true
  friend class D;

  template<class>
  requires true
  class E;
};

template<class>
requires true
class D {
  void f();
};

template<class T>
void D<T>::f() { } // { dg-error "class D' with different constraints" }

template<class>
requires true
class C::E {
  void f();
};

template<class T>
void C::E<T>::f() { } // { dg-error "class C::E' with different constraints" }
