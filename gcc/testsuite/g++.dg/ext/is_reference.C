// { dg-do compile { target c++11 } }

#define SA(X) static_assert((X),#X)

SA(!__is_reference(void));
SA(!__is_reference(int*));

SA(__is_reference(int&));
SA(__is_reference(const int&));
SA(__is_reference(volatile int&));
SA(__is_reference(const volatile int&));

SA(__is_reference(int&&));
SA(__is_reference(const int&&));
SA(__is_reference(volatile int&&));
SA(__is_reference(const volatile int&&));

SA(!__is_reference(int[3]));
SA(!__is_reference(const int[3]));
SA(!__is_reference(volatile int[3]));
SA(!__is_reference(const volatile int[3]));

SA(!__is_reference(int(int)));
SA(!__is_reference(int(*const)(int)));
SA(!__is_reference(int(*volatile)(int)));
SA(!__is_reference(int(*const volatile)(int)));
