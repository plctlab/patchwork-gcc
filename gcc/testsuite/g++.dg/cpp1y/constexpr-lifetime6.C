// { dg-do compile { target c++14 } }
// { dg-options "-Wno-return-local-addr" }

struct Empty {};

constexpr const Empty& empty() {
  return Empty{};
}

constexpr const Empty& empty_parm(Empty e) {
  return e;
}

constexpr Empty a = empty();  // { dg-error "" }
constexpr Empty b = empty_parm({});  // { dg-error "" }
