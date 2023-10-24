// PR c++/111929

template<class>
void f(long var) {
  new int[var + 42];
}
