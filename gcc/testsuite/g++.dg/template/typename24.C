// PR c++/107773

struct a {
  typedef void get;
};

struct b : a {
  int get(int i) const;
};

template<class T>
void f() {
  typedef typename T::get type; // { dg-error "'int b::get\\(int\\) const', which is not a type" }
}

template void f<b>();
