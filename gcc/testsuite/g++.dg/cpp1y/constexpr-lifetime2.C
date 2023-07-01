// PR c++/98675
// { dg-do compile { target c++14 } }

struct S {
  int x = 0;
  constexpr const int& get() const { return x; }
};

constexpr int error() {
  const auto& local = S{}.get();  // { dg-message "note: declared here" }
  return local;
}
constexpr int x = error();  // { dg-error "accessing object outside its lifetime" }

constexpr int ok() {
  // temporary should only be destroyed after end of full-expression
  auto local = S{}.get();
  return local;
}
constexpr int y = ok();
