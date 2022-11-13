// { dg-options "" }

void __attribute__((arm_streaming_compatible)) sc_a ();
void sc_a (); // { dg-error "conflicting types" }

void sc_b ();
void __attribute__((arm_streaming_compatible)) sc_b (); // { dg-error "conflicting types" }

void __attribute__((arm_streaming_compatible)) sc_c ();
void sc_c () {} // Inherits attribute from declaration (confusingly).

void sc_d ();
void __attribute__((arm_streaming_compatible)) sc_d () {} // { dg-error "conflicting types" }

void __attribute__((arm_streaming_compatible)) sc_e () {}
void sc_e (); // { dg-error "conflicting types" }

void sc_f () {}
void __attribute__((arm_streaming_compatible)) sc_f (); // { dg-error "conflicting types" }

extern void (*sc_g) ();
extern __attribute__((arm_streaming_compatible)) void (*sc_g) (); // { dg-error "conflicting types" }

extern __attribute__((arm_streaming_compatible)) void (*sc_h) ();
extern void (*sc_h) (); // { dg-error "conflicting types" }

//----------------------------------------------------------------------------

void __attribute__((arm_streaming)) s_a ();
void s_a (); // { dg-error "conflicting types" }

void s_b ();
void __attribute__((arm_streaming)) s_b (); // { dg-error "conflicting types" }

void __attribute__((arm_streaming)) s_c ();
void s_c () {} // Inherits attribute from declaration (confusingly).

void s_d ();
void __attribute__((arm_streaming)) s_d () {} // { dg-error "conflicting types" }

void __attribute__((arm_streaming)) s_e () {}
void s_e (); // { dg-error "conflicting types" }

void s_f () {}
void __attribute__((arm_streaming)) s_f (); // { dg-error "conflicting types" }

extern void (*s_g) ();
extern __attribute__((arm_streaming)) void (*s_g) (); // { dg-error "conflicting types" }

extern __attribute__((arm_streaming)) void (*s_h) ();
extern void (*s_h) (); // { dg-error "conflicting types" }

//----------------------------------------------------------------------------

void __attribute__((arm_streaming)) mixed_a ();
void __attribute__((arm_streaming_compatible)) mixed_a (); // { dg-error "conflicting types" }
// { dg-warning "ignoring attribute" "" { target *-*-* } .-1 }

void __attribute__((arm_streaming_compatible)) mixed_b ();
void __attribute__((arm_streaming)) mixed_b (); // { dg-error "conflicting types" }
// { dg-warning "ignoring attribute" "" { target *-*-* } .-1 }

void __attribute__((arm_streaming)) mixed_c ();
void __attribute__((arm_streaming_compatible)) mixed_c () {} // { dg-warning "ignoring attribute" }

void __attribute__((arm_streaming_compatible)) mixed_d ();
void __attribute__((arm_streaming)) mixed_d () {} // { dg-warning "ignoring attribute" }

void __attribute__((arm_streaming)) mixed_e () {}
void __attribute__((arm_streaming_compatible)) mixed_e (); // { dg-error "conflicting types" }
// { dg-warning "ignoring attribute" "" { target *-*-* } .-1 }

void __attribute__((arm_streaming_compatible)) mixed_f () {}
void __attribute__((arm_streaming)) mixed_f (); // { dg-error "conflicting types" }
// { dg-warning "ignoring attribute" "" { target *-*-* } .-1 }

extern __attribute__((arm_streaming_compatible)) void (*mixed_g) ();
extern __attribute__((arm_streaming)) void (*mixed_g) (); // { dg-error "conflicting types" }

extern __attribute__((arm_streaming)) void (*mixed_h) ();
extern __attribute__((arm_streaming_compatible)) void (*mixed_h) (); // { dg-error "conflicting types" }

//----------------------------------------------------------------------------

void __attribute__((arm_streaming, arm_streaming_compatible)) contradiction_1(); // { dg-warning "conflicts with attribute" }
void __attribute__((arm_streaming_compatible, arm_streaming)) contradiction_2(); // { dg-warning "conflicts with attribute" }

int __attribute__((arm_streaming_compatible)) int_attr; // { dg-warning "only applies to function types" }
void *__attribute__((arm_streaming)) ptr_attr; // { dg-warning "only applies to function types" }

typedef void __attribute__((arm_streaming)) s_callback ();
typedef void __attribute__((arm_streaming_compatible)) sc_callback ();

void (*__attribute__((arm_streaming)) s_callback_ptr) ();
void (*__attribute__((arm_streaming_compatible)) sc_callback_ptr) ();

typedef void __attribute__((arm_streaming, arm_streaming_compatible)) contradiction_callback_1 (); // { dg-warning "conflicts with attribute" }
typedef void __attribute__((arm_streaming_compatible, arm_streaming)) contradiction_callback_2 (); // { dg-warning "conflicts with attribute" }

void __attribute__((arm_streaming, arm_streaming_compatible)) (*contradiction_callback_ptr_1) (); // { dg-warning "conflicts with attribute" }
void __attribute__((arm_streaming_compatible, arm_streaming)) (*contradiction_callback_ptr_2) (); // { dg-warning "conflicts with attribute" }

struct s {
  void __attribute__((arm_streaming, arm_streaming_compatible)) (*contradiction_callback_ptr_1) (); // { dg-warning "conflicts with attribute" }
  void __attribute__((arm_streaming_compatible, arm_streaming)) (*contradiction_callback_ptr_2) (); // { dg-warning "conflicts with attribute" }
};
