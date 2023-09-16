#define CST 5
#define OP !=
#define op_eq ==
#define op_ne !=

#define function(vol,op, cst) \
__attribute__((noipa)) \
_Bool func_##op##_##cst##_##vol(vol int a, vol _Bool b) \
{ \
  vol int d = (a * b); \
  return d op_##op cst; \
}

#define funcdefs(op,cst) \
function(,op,cst) \
function(volatile,op,cst)

#define funcs(f) \
f(eq,0) \
f(eq,1) \
f(eq,5) \
f(ne,0) \
f(ne,1) \
f(ne,5)

funcs(funcdefs)

#define test(op,cst) \
do { \
 if(func_##op##_##cst##_(a,b) != func_##op##_##cst##_volatile(a,b))\
   __builtin_abort(); \
} while(0);

int main(void)
{
        for(int a = -10; a <= 10; a++)
        {
                _Bool b = 0;
                funcs(test)
                b = 1;
                funcs(test)
        }
}
