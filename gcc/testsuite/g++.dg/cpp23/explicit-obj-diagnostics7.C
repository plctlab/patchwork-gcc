// P0847R7
// { dg-do compile { target c++23 } }

// rejection and diagnosis when taking address of an unqualified xobj member function

struct S {
  static void static_f(S&) {}
  void iobj_member_f() {}
  void xobj_member_f(this S&) {}

  void test() {
    using func_ptr_type = void(*)(S&);
    // using mem_func_ptr_type = void (S::*)();

    // allowed (not testing for this)
    // func_ptr_type static_f_ptr = &static_f;

    // not allowed (also not testing for this)
    // mem_func_ptr_type iobj_mem_f_ptr = &iobj_member_f;

    // not allowed (this is what we are testing for)
    func_ptr_type xobj_mem_f_ptr = &xobj_member_f; // { dg-error "undetermined error" "disallowing address of unqualified explicit object member function is not implemented yet" { xfail *-*-* } }
  }
};
