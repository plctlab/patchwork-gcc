/* { dg-do compile } */
/* { dg-options "-march=rv64gc_zbb -mabi=lp64d -fno-lto -O2" } */
/* { dg-skip-if "" { *-*-* } { "-g" } } */
/* { dg-final { check-function-bodies "**" "" } } */

/*
**foo1:
**	rori	a0,a0,34
**	ret
*/
unsigned long foo1 (unsigned long rs1)
{ return (rs1 >> (34)) | (rs1 << 30); }

/*
**foo2:
**	rori	a0,a0,54
**	ret
*/
unsigned long foo2(unsigned long rs1)
{
    return (rs1 << 10) | (rs1 >> 54);
}

/*
**foo3:
**	roriw	a0,a0,20
**	ret
*/
unsigned int foo3(unsigned int rs1)
{
    return (rs1 >> 20) | (rs1 << 12);
}

/*
**foo4:
**	roriw	a0,a0,22
**	ret
*/
unsigned int foo4(unsigned int rs1)
{
    return (rs1 << 10) | (rs1 >> 22);
}

/*
**foo5:
**	rorw	a0,a0,a1
**	ret
*/
unsigned int foo5(unsigned int rs1, unsigned int rs2)
{
    return (rs1 >> rs2) | (rs1 << (32 - rs2));
}

/*
**foo6:
**	rori	a0,a0,32
**	ret
*/
unsigned long foo6(unsigned long rotate)
{
    return (rotate << 32) | (rotate >> 32);
}

/*
**foo7:
**	roriw	a0,a0,16
**	ret
*/
unsigned int foo7(unsigned int rotate)
{
    return (rotate << 16) | (rotate >> 16);
}
