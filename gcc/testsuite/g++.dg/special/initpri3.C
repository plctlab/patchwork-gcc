// Verify __has_attribute(init_priority) is false whenever the platform
// doesn't support it, and is treated as an unrecognized attribute.

#if !__has_attribute(init_priority)
#error init_priority /* { dg-error "" "" { target { ! init_priority } } } */
#endif

struct A { A(); } a __attribute__((init_priority(500)));
// { dg-warning "attribute directive ignored" "" { target { ! init_priority } } .-1 }
