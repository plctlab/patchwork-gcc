/* { dg-do run }
 * { dg-options "-fsanitize=vla-bound" } */

void foo(int n, char (*buf)[n], char p[n = 1])
{
}

int main()
{
	foo(-1, 0, 0);
}

/* { dg-output "variable length array bound evaluates to non-positive value -1" } */

