/* { dg-do compile }
 * { dg-options "-std=c23" }
 */

// bit-fields

extern struct foo { int x:3; } x;
struct foo { int x:3; } y;

void f(void)
{
	extern typeof(*(1 ? &x : &y)) x;
	&x.x;					/* { dg-error "bit-field" } */
}

struct foo { int x:2; };			/* { dg-error "redefinition" } */

