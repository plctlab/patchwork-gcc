// PR c++/99599
// { dg-do compile { target c++20 } }

struct foo_tag {
  foo_tag() = default;
  foo_tag(int);
  operator int();
};

struct bar_tag {
  bar_tag() = default;
  bar_tag(int);
  operator int();
};

template <class T>
concept fooable = requires(T it) {
  invoke_tag(foo_tag{}, it);
};

template<class T>
void invoke_tag(foo_tag, T in);

template<fooable T>
void invoke_tag(bar_tag, T it);

int main() {
  invoke_tag(foo_tag{}, 2);
  invoke_tag(bar_tag{}, 2);
}
