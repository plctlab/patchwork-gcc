// P0847R7
// { dg-do compile { target c++23 } }

// recursive lambdas

// { dg-excess-errors "deducing this with lambdas not implemented yet" { xfail *-*-* } }

int main()
{
  auto cl0 = [](this auto&& self, int n){ return n ? self(n - 1) : 42 };
  auto cl1 = [](this auto self, int n){ return n ? self(n - 1) : 42};
  int a = cl0(5);
  int b = cl1(5);
}

