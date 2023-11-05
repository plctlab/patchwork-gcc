// P0847R7
// { dg-do compile { target c++20_down } }
// { dg-options "-Wno-error=pedantic" }

struct S {
    void f(this S); // { dg-warning {explicit object member function only available with '-std=c\+\+23' or '-std=gnu\+\+23'} }
};
