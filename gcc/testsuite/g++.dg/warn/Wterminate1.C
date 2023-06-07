// In C++98 mode this gets a -Wc++11-compat warning, in C++11 mode a
// -Wterminate warning.

// { dg-require-effective-target exceptions_enabled }
// { dg-options "-Wall" }

struct A
{
  ~A()
  {
    throw 1;			// { dg-warning "terminate" }
  }
};

int main()
{
  try { A a;  }
  catch (...) {}
}


