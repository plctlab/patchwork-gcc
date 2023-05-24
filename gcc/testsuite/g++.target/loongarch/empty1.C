/* { dg-do compile } */
/* { dg-options "-O2 -mabi=lp64d" } */
/* { dg-final { scan-assembler-times "ld.bu\t\\\$r4,\\\$r12,0" 1 } } */

struct E {};
struct s1
{
  struct E {} e1;
  float f0;
};

extern void fun (struct s1);

struct s1 s1_1 = {{}, 1.0};
void test (void)
{
  fun (s1_1);
}
