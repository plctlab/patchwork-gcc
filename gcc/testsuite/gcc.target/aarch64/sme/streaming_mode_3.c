// { dg-options "" }

#pragma GCC target "+nosme"

void __attribute__((arm_streaming_compatible)) sc_a () {}
void __attribute__((arm_streaming)) s_a () {} // { dg-error "streaming functions require the ISA extension 'sme'" }
void ns_a () {}

void __attribute__((arm_streaming_compatible)) sc_b () {}
void ns_b () {}
void __attribute__((arm_streaming)) s_b () {} // { dg-error "streaming functions require the ISA extension 'sme'" }

void __attribute__((arm_streaming_compatible)) sc_c () {}
void __attribute__((arm_streaming_compatible)) sc_d () {}

void __attribute__((arm_streaming)) s_c () {} // { dg-error "streaming functions require the ISA extension 'sme'" }
void __attribute__((arm_streaming)) s_d () {} // { dg-error "streaming functions require the ISA extension 'sme'" }

void ns_c () {}
void ns_d () {}

void __attribute__((arm_streaming_compatible)) sc_e ();
void __attribute__((arm_streaming)) s_e ();
void ns_e ();

#pragma GCC target "+sme"

void __attribute__((arm_streaming_compatible)) sc_f () {}
void __attribute__((arm_streaming)) s_f () {}
void ns_f () {}

void __attribute__((arm_streaming_compatible)) sc_g () {}
void ns_g () {}
void __attribute__((arm_streaming)) s_g () {}

void __attribute__((arm_streaming_compatible)) sc_h () {}
void __attribute__((arm_streaming_compatible)) sc_i () {}

void __attribute__((arm_streaming)) s_h () {}
void __attribute__((arm_streaming)) s_i () {}

void ns_h () {}
void ns_i () {}

void __attribute__((arm_streaming_compatible)) sc_j ();
void __attribute__((arm_streaming)) s_j ();
void ns_j ();

#pragma GCC target "+sme"

void __attribute__((arm_streaming_compatible)) sc_k () {}

#pragma GCC target "+nosme"
#pragma GCC target "+sme"

void __attribute__((arm_streaming)) s_k () {}

#pragma GCC target "+nosme"
#pragma GCC target "+sme"

void ns_k () {}

#pragma GCC target "+nosme"
