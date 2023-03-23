// PR c++/106880
// Verify we don't emit a "use of deleted function" diagnostic twice.
// { dg-do compile { target c++11 } }

void foo() = delete;

template<class T>
void f(T t) { foo(t); } // { dg-bogus "deleted function.*deleted function" }
                        // { dg-error "deleted function" "" { target *-*-*} .-1 }

template void f(int);
