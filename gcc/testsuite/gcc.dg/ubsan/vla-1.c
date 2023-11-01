/* { dg-do run } */
/* { dg-options "-Wstringop-overflow=0 -fsanitize=vla-bound -fsanitize-recover=vla-bound" } */

void f1(int n, char buf[n]) { }
/* { dg-output "variable length array bound evaluates to non-positive value -1\[^\n\r]*(\n|\r\n|\r)" } */
void f2(int n, float m[n][2]) { }
/* { dg-output "\[^\n\r]*variable length array bound evaluates to non-positive value -1\[^\n\r]*(\n|\r\n|\r)" } */
void f3(int n, int m, float x[n][m]) { }
/* { dg-output "\[^\n\r]*variable length array bound evaluates to non-positive value -1\[^\n\r]*(\n|\r\n|\r)" } */
void f4(int n) { typedef char buf[n]; }
/* { dg-output "\[^\n\r]*variable length array bound evaluates to non-positive value -1\[^\n\r]*(\n|\r\n|\r)" } */
void f5(int n) { struct { char buf[n]; } x; }
/* { dg-output "\[^\n\r]*variable length array bound evaluates to non-positive value -1\[^\n\r]*(\n|\r\n|\r)" } */
void f6(int n, struct { char buf[n]; } *p) { }	/* { dg-warning "anonymous" } */
/* { dg-output "\[^\n\r]*variable length array bound evaluates to non-positive value -1" } */

int main()
{
	f1(-1, 0);
	f2(-1, 0);
	f3(-1, 1, 0);
	f4(-1);
	f5(-1);
	f6(-1, 0);
}


