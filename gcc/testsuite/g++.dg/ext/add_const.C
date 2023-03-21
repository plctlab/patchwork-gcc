// { dg-do compile { target c++11 } }

#define SA(X) static_assert((X),#X)

SA(__is_same(__add_const(void), const void));
SA(__is_same(__add_const(int), const int));

SA(__is_same(__add_const(const int), const int));
SA(__is_same(__add_const(volatile int), const volatile int));
SA(__is_same(__add_const(const volatile int), const volatile int));

SA(__is_same(__add_const(int*), int* const));
SA(__is_same(__add_const(int* const), int* const));
SA(__is_same(__add_const(int* volatile), int* const volatile));
SA(__is_same(__add_const(int* const volatile), int* const volatile));

SA(__is_same(__add_const(const int*), const int* const));
SA(__is_same(__add_const(volatile int*), volatile int* const));
SA(__is_same(__add_const(const volatile int*), const volatile int* const));

SA(__is_same(__add_const(int&), int&));
SA(__is_same(__add_const(const int&), const int&));
SA(__is_same(__add_const(volatile int&), volatile int&));
SA(__is_same(__add_const(const volatile int&), const volatile int&));

SA(__is_same(__add_const(int&&), int&&));
SA(__is_same(__add_const(const int&&), const int&&));
SA(__is_same(__add_const(volatile int&&), volatile int&&));
SA(__is_same(__add_const(const volatile int&&), const volatile int&&));

SA(__is_same(__add_const(int[3]), const int[3]));
SA(__is_same(__add_const(const int[3]), const int[3]));
SA(__is_same(__add_const(volatile int[3]), const volatile int[3]));
SA(__is_same(__add_const(const volatile int[3]), const volatile int[3]));

SA(__is_same(__add_const(int(int)), int(int)));
SA(__is_same(__add_const(int(*const)(int)), int(*const)(int)));
SA(__is_same(__add_const(int(*volatile)(int)), int(*volatile)(int)));
SA(__is_same(__add_const(int(*const volatile)(int)), int(*const volatile)(int)));
