// { dg-do compile { target c++14 } }
// { dg-options "-Wno-return-local-addr" }

constexpr const int& id(int x) { return x; }  // { dg-message "note: declared here" }

constexpr bool test() {
  const int& y = id(3);
  return y == 3;  // { dg-error "accessing object outside its lifetime" }
}

constexpr bool x = test();  // { dg-message "in .constexpr. expansion" }
