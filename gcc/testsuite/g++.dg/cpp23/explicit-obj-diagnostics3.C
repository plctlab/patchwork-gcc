// P0847R7
// { dg-do compile { target c++23 } }

// rejection and diagnosis of equivalent redeclarations of member functions

// xobj redeclared as iobj

struct S0 {
  void f0(this S0&); // { dg-note "previous declaration" }
  void f0(); // { dg-error "cannot be overloaded with" }

  void f1(this S0 const&); // { dg-note "previous declaration" "detecting redeclarations of xobj member functions as iobj member functions without a ref qualifier is known to be broken" { xfail *-*-* } }
  void f1() const; // { dg-error "cannot be overloaded with" "detecting redeclarations of xobj member functions as iobj member functions without a ref qualifier is known to be broken" { xfail *-*-* } }

  void f2(this S0 volatile&); // { dg-note "previous declaration" "detecting redeclarations of xobj member functions as iobj member functions without a ref qualifier is known to be broken" { xfail *-*-* } }
  void f2() volatile; // { dg-error "cannot be overloaded with" "detecting redeclarations of xobj member functions as iobj member functions without a ref qualifier is known to be broken" { xfail *-*-* } }

  void f3(this S0 const volatile&); // { dg-note "previous declaration" "detecting redeclarations of xobj member functions as iobj member functions without a ref qualifier is known to be broken" { xfail *-*-* } }
  void f3() const volatile; // { dg-error "cannot be overloaded with" "detecting redeclarations of xobj member functions as iobj member functions without a ref qualifier is known to be broken" { xfail *-*-* } }

  void f4(this S0&); // { dg-note "previous declaration" }
  void f4() &; // { dg-error "cannot be overloaded with" }

  void f5(this S0&&); // { dg-note "previous declaration" }
  void f5() &&; // { dg-error "cannot be overloaded with" }

  void f6(this S0 const&); // { dg-note "previous declaration" }
  void f6() const&; // { dg-error "cannot be overloaded with" }

  void f7(this S0 const&&); // { dg-note "previous declaration" }
  void f7() const&&; // { dg-error "cannot be overloaded with" }

  void f8(this S0 volatile&); // { dg-note "previous declaration" }
  void f8() volatile&; // { dg-error "cannot be overloaded with" }

  void f9(this S0 volatile&&); // { dg-note "previous declaration" }
  void f9() volatile&&; // { dg-error "cannot be overloaded with" }

  void fA(this S0 const volatile&); // { dg-note "previous declaration" }
  void fA() const volatile&; // { dg-error "cannot be overloaded with" }

  void fB(this S0 const volatile&&); // { dg-note "previous declaration" }
  void fB() const volatile&&; // { dg-error "cannot be overloaded with" }
};

// iobj redeclared as xobj

struct S1 {
  void f0(); // { dg-note "previous declaration" }
  void f0(this S1&); // { dg-error "cannot be overloaded with" }

  void f1() const; // { dg-note "previous declaration" "detecting redeclarations of iobj member functions without a ref qualifier as xobj member functions is known to be broken" { xfail *-*-* } }
  void f1(this S1 const&); // { dg-error "cannot be overloaded with" "detecting redeclarations of iobj member functions without a ref qualifier as xobj member functions is known to be broken" { xfail *-*-* } }

  void f2() volatile; // { dg-note "previous declaration" "detecting redeclarations of iobj member functions without a ref qualifier as xobj member functions is known to be broken" { xfail *-*-* } }
  void f2(this S1 volatile&); // { dg-error "cannot be overloaded with" "detecting redeclarations of iobj member functions without a ref qualifier as xobj member functions is known to be broken" { xfail *-*-* } }

  void f3() const volatile; // { dg-note "previous declaration" "detecting redeclarations of iobj member functions without a ref qualifier as xobj member functions is known to be broken" { xfail *-*-* } }
  void f3(this S1 const volatile&); // { dg-error "cannot be overloaded with" "detecting redeclarations of iobj member functions without a ref qualifier as xobj member functions is known to be broken" { xfail *-*-* } }

  void f4() &; // { dg-note "previous declaration" }
  void f4(this S1&); // { dg-error "cannot be overloaded with" }

  void f5() &&; // { dg-note "previous declaration" }
  void f5(this S1&&); // { dg-error "cannot be overloaded with" }

  void f6() const&; // { dg-note "previous declaration" }
  void f6(this S1 const&); // { dg-error "cannot be overloaded with" }

  void f7() const&&; // { dg-note "previous declaration" }
  void f7(this S1 const&&); // { dg-error "cannot be overloaded with" }

  void f8() volatile&; // { dg-note "previous declaration" }
  void f8(this S1 volatile&); // { dg-error "cannot be overloaded with" }

  void f9() volatile&&; // { dg-note "previous declaration" }
  void f9(this S1 volatile&&); // { dg-error "cannot be overloaded with" }

  void fA() const volatile&; // { dg-note "previous declaration" }
  void fA(this S1 const volatile&); // { dg-error "cannot be overloaded with" }

  void fB() const volatile&&; // { dg-note "previous declaration" }
  void fB(this S1 const volatile&&); // { dg-error "cannot be overloaded with" }
};
