/* { dg-do compile } */
/* { dg-options "-O2 -mbranch-protection=pac-ret+leaf" } */
/* { dg-final { check-function-bodies "**" "" "" } } */

/*
**foo:
**	hint	25 // paciasp
**	stp	x0, x1, .*
**	stp	x2, x3, .*
**	cbz	w2, .*
**	mov	x4, 0
**	ldp	x2, x3, .*
**	ldp	x0, x1, .*
**	cbz	x4, .*
**	add	sp, sp, x5
**	br	x6
**	hint	29 // autiasp
**	ret
**	mov	x5, x0
**	mov	x6, x1
**	mov	x4, 1
**	b	.*
*/
void
foo (unsigned long off, void *handler, int c)
{
  if (c)
    return;
  __builtin_eh_return (off, handler);
}
