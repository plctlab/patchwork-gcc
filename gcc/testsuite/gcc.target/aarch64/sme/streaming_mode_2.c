// { dg-options "" }

void __attribute__((arm_streaming_compatible)) sc_fn ();
void __attribute__((arm_streaming)) s_fn ();
void ns_fn ();

__attribute__((arm_streaming_compatible)) void (*sc_fn_ptr) ();
__attribute__((arm_streaming)) void (*s_fn_ptr) ();
void (*ns_fn_ptr) ();

void
f ()
{
  sc_fn_ptr = sc_fn;
  sc_fn_ptr = s_fn; // { dg-warning "incompatible pointer type" }
  sc_fn_ptr = ns_fn; // { dg-warning "incompatible pointer type" }

  s_fn_ptr = sc_fn; // { dg-warning "incompatible pointer type" }
  s_fn_ptr = s_fn;
  s_fn_ptr = ns_fn; // { dg-warning "incompatible pointer type" }

  ns_fn_ptr = sc_fn; // { dg-warning "incompatible pointer type" }
  ns_fn_ptr = s_fn; // { dg-warning "incompatible pointer type" }
  ns_fn_ptr = ns_fn;
}
