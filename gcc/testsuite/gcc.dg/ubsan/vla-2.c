/* { dg-do run } */
/* { dg-options "-Wstringop-overflow=0 -fsanitize=vla-bound -fsanitize-recover=vla-bound" } */

void f1(int n, char buf[n]) { }
void f2(int n, float m[n][2]) { }
void f3(int n, int m, float x[n][m]) { }
void f4(int n) { typedef char buf[n]; }
void f5(int n) { struct { char buf[n]; } x; }
void f6(int n, struct { char buf[n]; } *p) { }	/* { dg-warning "anonymous" } */

int main()
{
	f1(0, 0);
	f2(0, 0);
	f3(0, 1, 0);
	f4(0);
	f5(0);
	f6(0, 0);
}


