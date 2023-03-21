// { dg-do compile { target c++11 } }

#include <testsuite_tr1.h>

using namespace __gnu_test;

#define SA(X) static_assert((X),#X)

struct A
{ void fn(); };

template<typename>
struct AHolder { };

template<class T, class U>
struct AHolder<U T::*>
{ using type = U; };

// Positive tests.
SA(__is_function(int (int)));
SA(__is_function(ClassType (ClassType)));
SA(__is_function(float (int, float, int[], int&)));
SA(__is_function(int (int, ...)));
SA(__is_function(bool (ClassType) const));
SA(__is_function(AHolder<decltype(&A::fn)>::type));

void fn();
SA(__is_function(decltype(fn)));

// Negative tests.
SA(!__is_function(int));
SA(!__is_function(int*));
SA(!__is_function(int&));
SA(!__is_function(void));
SA(!__is_function(const void));
SA(!__is_function(void*));
SA(!__is_function(const void*));
SA(!__is_function(void**));
SA(!__is_function(std::nullptr_t));

SA(!__is_function(AbstractClass));
SA(!__is_function(int(&)(int)));
SA(!__is_function(int(*)(int)));

SA(!__is_function(A));
SA(!__is_function(decltype(&A::fn)));

struct FnCallOverload
{ void operator()(); };
SA(!__is_function(FnCallOverload));

// Sanity check.
SA(!__is_function(ClassType));
SA(!__is_function(IncompleteClass));
SA(!__is_function(IncompleteUnion));
