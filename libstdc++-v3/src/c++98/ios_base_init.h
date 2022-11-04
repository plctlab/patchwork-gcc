// This is only in a header so we can use the system_header pragma,
// to suppress the warning caused by using a reserved init_priority.
#pragma GCC system_header

#if !_GLIBCXX_HAVE_ATTRIBUTE_INIT_PRIORITY
# error "This file should not be included for this build"
#endif

static ios_base::Init __ioinit __attribute__((init_priority(90)));
