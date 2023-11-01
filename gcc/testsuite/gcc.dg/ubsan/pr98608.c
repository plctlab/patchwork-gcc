/* PR 98608 */
/* { dg-do run } */
/* { dg-options "-fsanitize=vla-bound" } */

void f(int n, double (*x)[n])
{
	typeof(*x) y;	
}

int main()
{
	f(-1, 0);
}

/* { dg-output "variable length array bound evaluates to non-positive value -1" } */

