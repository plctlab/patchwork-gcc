// PR c++/105644
// { dg-do compile { target c++20 } }

template<class T, class U>
concept same_as = __is_same(T, U);

template<class... Ts>
concept C = (requires { { Ts() } -> same_as<Ts>; } && ...);

static_assert(C<int, char>);
static_assert(!C<int, const char>);
static_assert(!C<const int, char>);
