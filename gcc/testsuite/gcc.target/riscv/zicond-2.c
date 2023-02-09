/* { dg-do compile } */
/* { dg-options "-march=rv64i_zicond -mabi=lp64" } */

void foo1()
{
    __builtin_riscv_zicond_nez(1,1);
}

void foo2()
{
    __builtin_riscv_zicond_eqz(1,1);
}

/* { dg-final { scan-assembler-times "czero.nez" 1 } } */ 
/* { dg-final { scan-assembler-times "czero.eqz" 1 } } */ 
