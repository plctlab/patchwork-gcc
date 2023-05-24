/* { dg-do compile } */
/* { dg-options "-O2 -mabi=lp64d" } */
/* { dg-final { scan-assembler-not "fld.d" } } */

struct E {};
struct s1
{
  struct E {} e1;
  double f0;
  double f1;
};

extern void fun (struct s1);

struct s1 s1_1 = {{}, 1.0, 2.0};
void test (void)
{
  fun (s1_1);
}
