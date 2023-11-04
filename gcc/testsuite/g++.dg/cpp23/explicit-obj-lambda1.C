// P0847R7
// { dg-do compile { target c++23 } }

// lambda declaration with xobj parameter

// { dg-excess-errors "explicit object parameter with lambdas not implemented yet" { xfail *-*-* } }

void test()
{
  (void)[](this auto&& self){};
}
