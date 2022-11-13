// { dg-options "-mgeneral-regs-only" }

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
