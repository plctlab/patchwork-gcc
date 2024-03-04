// { dg-do compile }
// { dg-options "-O2 -fno-pic -mfdpic" }
// { dg-skip-if "-mpure-code and -fPIC incompatible" { *-*-* } { "-mpure-code" } }

__attribute__((visibility("hidden"))) void hidden_fun(void);
void fun(void);
__attribute__((visibility("hidden"))) extern int hidden_var;
extern int var;
__attribute__((visibility("hidden"))) const int ro_hidden_var = 42;

// { dg-final { scan-assembler "hidden_fun\\(GOTOFFFUNCDESC\\)" } }
void *addr_hidden_fun(void) { return hidden_fun; }

// { dg-final { scan-assembler "fun\\(GOTFUNCDESC\\)" } }
void *addr_fun(void) { return fun; }

// { dg-final { scan-assembler "hidden_var\\(GOT\\)" } }
void *addr_hidden_var(void) { return &hidden_var; }

// { dg-final { scan-assembler "var\\(GOT\\)" } }
void *addr_var(void) { return &var; }

// { dg-final { scan-assembler ".LANCHOR0\\(GOT\\)" } }
const int *addr_ro_hidden_var(void) { return &ro_hidden_var; }

// { dg-final { scan-assembler "hidden_var\\(GOT\\)" } }
int read_hidden_var(void) { return hidden_var; }

// { dg-final { scan-assembler "var\\(GOT\\)" } }
int read_var(void) { return var; }
