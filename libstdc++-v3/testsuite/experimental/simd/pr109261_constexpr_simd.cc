// { dg-options "-std=gnu++17" }
// { dg-do compile { target c++17 } }
// { dg-require-cmath "" }

// Copyright (C) 2020-2023 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

#include <experimental/simd>

namespace stdx = std::experimental;

template <typename T, typename V>
  void
  test01()
  {
    constexpr T data[V::size()] = {};
    constexpr auto a = V(data, stdx::element_aligned);

    constexpr auto b = []() constexpr {
      V x = T(1);
      where(x > T(), x) = T();
      where(x < T(), x) += T();
      where(x >= T(), x) -= T();
      where(x <= T(), x) *= T();
      where(x == T(), x) /= T(1);
      where(x != T(), x) += T(1);
      return x;
    }();

    constexpr T c = V()[0];

    constexpr auto d = !V() && !!V() || !V() & !V() | !V() ^ !V();

    constexpr auto e = []() constexpr {
      T data[V::size()] = {};
      V(T(1)).copy_to(data, stdx::element_aligned);
      V x = T();
      x[0] = T(1);
      x.copy_from(data, stdx::element_aligned);
      bool mask[V::size()] = {};
      auto k = hmin(x + x - x * x) == x / x;
      k.copy_to(mask, stdx::element_aligned);
      mask[0] = false;
      using M = typename V::mask_type;
      return M(mask, stdx::element_aligned);
    }();

    static_assert(not e[0]);
    static_assert(popcount(e) == V::size() - 1);

    static_assert(all_of(V(T(1)) == []() constexpr {
      float data[V::size()] = {};
      V(T(1)).copy_to(data, stdx::element_aligned);
      V x = T();
      x.copy_from(data, stdx::element_aligned);
      return x;
    }()));

    static_assert(hmin(V()) == T());
    static_assert(hmax(V()) == T());
    static_assert(reduce(V(1)) == T(V::size()));
  }

template <typename T>
  void
  iterate_abis()
  {
    test01<T, stdx::simd<T, stdx::simd_abi::scalar>>();
    test01<T, stdx::simd<T>>();
    test01<T, stdx::native_simd<T>>();
    test01<T, stdx::fixed_size_simd<T, 3>>();
    test01<T, stdx::fixed_size_simd<T, stdx::simd_abi::max_fixed_size<T> - 4>>();
  }

int main()
{
  iterate_abis<char>();
  iterate_abis<wchar_t>();
  iterate_abis<char16_t>();
  iterate_abis<char32_t>();

  iterate_abis<signed char>();
  iterate_abis<unsigned char>();
  iterate_abis<short>();
  iterate_abis<unsigned short>();
  iterate_abis<int>();
  iterate_abis<unsigned int>();
  iterate_abis<long>();
  iterate_abis<unsigned long>();
  iterate_abis<long long>();
  iterate_abis<unsigned long long>();
  iterate_abis<float>();
  iterate_abis<double>();
  iterate_abis<long double>();
}
