// DR 1693, Superfluous semicolons in class definitions
// PR c++/113760
// { dg-do compile }
// { dg-options "-pedantic-errors" }
// { dg-prune-output "only available with" }

struct X { ; };		      // { dg-error "extra .;. inside a struct" "" { target c++11_down } }

struct S {
  void baz () = delete;
  void qux () = delete;
  ;			      // { dg-error "extra .;. inside a struct" "" { target c++11_down } }
  void corge () = delete;
  ;			      // { dg-error "extra .;. inside a struct" "" { target c++11_down } }
  ;			      // { dg-error "extra .;. inside a struct" "" { target c++11_down } }
  int s;
  ;			      // { dg-error "extra .;. inside a struct" "" { target c++11_down } }
};
