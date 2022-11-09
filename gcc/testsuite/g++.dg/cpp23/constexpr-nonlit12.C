// PR c++/106649
// P2448 - Relaxing some constexpr restrictions
// { dg-do compile { target c++23 } }
// Test that we get a diagnostic even in C++23 if you do call the function.

constexpr unsigned int
fn0 (const int *p)
{
  return *reinterpret_cast<unsigned const int *>(p); // { dg-error ".reinterpret_cast. is not a constant expression" }
}

constexpr void *
fn1 (int i)
{
  return (void *) 1LL;
}

void
g ()
{
  constexpr int i = 42;
  /* The diagnostics differ.  fn0 is considered potentially-constant, but fn1
     isn't due to "reinterpret_cast from integer to pointer".  So for fn1,
     maybe_save_constexpr_fundef doesn't register_constexpr_fundef because
     'potential' is false.  Then cxx_eval_call_expression issues the "called
     in a constant expression" error, and explain_invalid_constexpr_fn doesn't
     explain what the problem is because it has "Only diagnose defaulted
     functions, lambdas, or instantiations."  (With -Winvalid-constexpr, we
     emit more information.)  */
  constexpr auto a1 = fn0 (&i);
  constexpr auto a2 = fn1 (i); // { dg-error "called in a constant expression" }
}
