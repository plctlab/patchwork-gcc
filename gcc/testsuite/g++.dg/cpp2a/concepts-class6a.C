// PR c++/109655
// { dg-do compile { target c++20 } }

template<class T>
requires true
class C {
  class D;

  template<class>
  requires (!!true)
  class E;
};

template<class T>
requires true
class C<T>::D {
  void f();
};

template<class T>  // missing "requires true"
void C<T>::D::f() { } // { dg-error "class C' with different constraints" }

template<class T>
requires true
template<class U>
requires (!!true)
class C<T>::E {
  void f();
  void g();
};

template<class T>
requires true
template<class U>
void C<T>::E<U>::f() { } // { dg-error "class C<T>::E' with different constraints" }

template<class T>
template<class U>
requires (!!true)
void C<T>::E<U>::g() { } // { dg-error "class C' with different constraints" }
