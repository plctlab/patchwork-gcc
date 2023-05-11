/* { dg-do compile { target { ! ia32 } } } */
/* { dg-options "-Os" } */
#define $expr(...) (__extension__({__VA_ARGS__;}))
#define $regF0 $expr(register double x __asm("xmm0"); x)
#define $regF1 $expr(register double x __asm("xmm1"); x)
#define $regF2 $expr(register double x __asm("xmm2"); x)
#define $regF3 $expr(register double x __asm("xmm3"); x)
#define $regF4 $expr(register double x __asm("xmm4"); x)
#define $regF5 $expr(register double x __asm("xmm5"); x)
#define $regF6 $expr(register double x __asm("xmm6"); x)
#define $regF7 $expr(register double x __asm("xmm7"); x)

void func(char const*Fmt, ...);
void callfunc(char const*Fmt, double D0, double D1, double D2, double D3,
                              double D4, double D5, double D6, double D7){
    func(Fmt,$regF0,$regF1,$regF2,$regF3,$regF4,$regF5,$regF6,$regF7,
            D0,D1,D2,D3,D4,D5,D6,D7);
}

/* { dg-final { scan-assembler-times "pushq" 9 } } */
/* { dg-final { scan-assembler-not "leaq" } } */
