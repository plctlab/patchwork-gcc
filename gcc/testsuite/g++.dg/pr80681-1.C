// PR c++/80681
// { dg-do compile }
// { dg-options "-Wuninitialized -c" }

/* Status quo: the original should still warn correctly.  */
struct StatusQuo
{
private:
  const int a; // { dg-warning "non-static const member" }
  int &b; // { dg-warning "non-static reference" }
};

/* Single layer of inheritance.  */
struct A {
  const int a; // { dg-warning "non-static const member" }
  int &b; // { dg-warning "non-static reference" }
};

struct B: private A {
};

/* multiple inheritance example.  */
struct X
{
  const int x; // { dg-warning "non-static const member" }
};

struct Y
{
  const int &y; // { dg-warning "non-static reference" }
};

struct Z : private Y, private X
{
};

/* multi-level inheritance example.  */
struct M
{
  const int x; // { dg-warning "non-static const member" }
};

struct N: private M
{
  const int &y; // { dg-warning "non-static reference" }
};

struct O: private N
{
};

