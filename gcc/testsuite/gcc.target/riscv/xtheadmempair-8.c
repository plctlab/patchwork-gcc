/* { dg-do compile } */
/* { dg-skip-if "" { *-*-* } { "-O0" "-O1" "-g" "-Oz" "-Os" "-funroll-loops"} } */
/* { dg-options "-march=rv64gc_xtheadmempair -mtune=thead-c906" { target { rv64 } } } */
/* { dg-options "-march=rv32gc_xtheadmempair -mtune=thead-c906" { target { rv32 } } } */

typedef int VINT32 __attribute__ ((vector_size((16))));

/* Note: When compiling for RV32, 321r.sched2 changes the ordering of some
   loads/stores sequences (after the peephole passes), which disables these
   optimization opportunities.  */

void
memory_operation (void * __restrict src, void * __restrict dest, int num)
{
  VINT32 *vsrc = (VINT32 *) src;
  VINT32 *vdest = (VINT32 *) dest;
  int i;

  for (i = 0; i < num - 1; i += 2)
  {
    vdest[i] = vdest[i] + vsrc[i];
    vdest[i + 1] = vdest[i + 1] + vsrc[i + 1];
  }
}

/* { dg-final { scan-assembler-times "th.lwd\t" 8 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.sdd\t" 2 { target { rv64 } } } } */
/* { dg-final { scan-assembler-times "th.lwd\t" 4 { target { rv32 } } } } */
/* { dg-final { scan-assembler-times "th.swd\t" 4 { target { rv32 } } } } */
