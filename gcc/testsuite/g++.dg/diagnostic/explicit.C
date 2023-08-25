// { dg-do compile { target c++11 } }

struct S {
  explicit S(int) { }
  explicit operator bool() const { return true; } // { dg-message "explicit conversion function was not considered" }
  explicit operator int() const { return 42; } // { dg-message "explicit conversion function was not considered" }
};

void
g ()
{
  S s = {1}; // { dg-error "would use explicit constructor" }
  bool b = S{1}; // { dg-error "cannot convert .S. to .bool. in initialization" }
  int i;
  i = S{2}; // { dg-error "cannot convert .S. to .int. in assignment" }
}
