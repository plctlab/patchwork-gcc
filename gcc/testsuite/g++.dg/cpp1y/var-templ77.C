// PR c++/107939
// { dg-do compile { target c++14 } }

struct Q {
  struct P {
    const Q* p;
  };
  int n;
  constexpr P operator()(int) const { return {this}; }
};

extern const Q q;
template<int>
constexpr auto p = q(0);
