/* { dg-do run } */
/* { dg-require-effective-target p9vector_hw } */

#include <altivec.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#define MASK 0x3
#define EXPECTED1 0x1
#define EXPECTED2 0x2

void abort (void);

int
main()
{
  unsigned long mask, result, expected;
  double double_arg;
  
  union convert_t {
    double d;
    unsigned long ul;
  } val;

  /* Test immediate version of __builtin_mffscrn. */
  /* Read FPSCR and set RN bits in FPSCR[62:63]. */
  val.d = __builtin_mffscrn (EXPECTED2);

  /* Read FPSCR, bits [62:63] should have been set to 0x2 by previous builtin
     call.  */
  val.d = __builtin_mffscrn (EXPECTED1);
  /* The expected result is the argument for the previous call to
     __builtin_mffscrn.  */
  expected = EXPECTED2;
  result = MASK & val.ul;

  if (EXPECTED2 != result)
#ifdef DEBUG
    printf("Result of mffscrn immediate doesn't match EXPECTED2.  Result was 0x%lx\n",
	   result);
#else
    abort();
#endif

  /* Read FPSCR, bits [62:63] should have been set to 0x1 by previous builtin
   call*/
  val.d = __builtin_mffscrn (EXPECTED1);
  expected = EXPECTED1;
  result = MASK & val.ul;

  if (EXPECTED1 != result)
#ifdef DEBUG
  printf("Result of mffscrn immediate doesn't match EXPECTED1.  Result was 0x%lx\n",
	 result);
#else
  abort();
#endif


  /* Test double argument version of __builtin_mffscrn */
  val.ul = EXPECTED2;
  double_arg = val.d;

  /* Read FPSCR and set RN bits in FPSCR[62:63]. */
  val.d = __builtin_mffscrn (double_arg);

  /* Read FPSCR, bits [62:63] should have been set to 0x2 by previous builtin  
     call.  */

  val.ul = EXPECTED1;
  double_arg = val.d;

  val.d = __builtin_mffscrn (double_arg);
  /* The expected result is the argument for the previous call to
     __builtin_mffscrn.  */
  expected = EXPECTED2;
  result = MASK & val.ul;

  if (EXPECTED2 != result)
#ifdef DEBUG
    printf("Result of mffscrn double arg doesn't match EXPECTED2.  Result was 0x%lx\n",
	   result);
#else
    abort();
#endif

  /* Read FPSCR, bits [62:63] should have been set to 0x1 by previous builtin
   call*/
  val.ul = EXPECTED1;
  double_arg = val.d;

  val.d = __builtin_mffscrn (double_arg);
  expected = EXPECTED1;
  result = MASK & val.ul;

  if (EXPECTED1 != result)
#ifdef DEBUG
    printf("Result of mffscrn double arg doesn't match EXPECTED1.  Result was 0x%lx\n",
	   result);
#else
    abort();
#endif
}
