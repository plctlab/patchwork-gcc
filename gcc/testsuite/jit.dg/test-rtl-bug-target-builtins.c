/* { dg-do compile { target x86_64-*-* } } */

#include <stdlib.h>
#include <stdio.h>

#include "libgccjit.h"

#define TEST_PROVIDES_MAIN
#include "harness.h"

void
create_code (gcc_jit_context *ctxt, void *user_data)
{
  gcc_jit_context_add_command_line_option (ctxt, "-mavx512vl");
  gcc_jit_function *builtin =
    gcc_jit_context_get_target_builtin_function (ctxt,
        "__builtin_ia32_cvtpd2udq128_mask");

  gcc_jit_type *u8_type =
    gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_UINT8_T);
  gcc_jit_type *double_type =
    gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_DOUBLE);
  gcc_jit_type *v2df = gcc_jit_type_get_vector (double_type, 2);
  gcc_jit_type *int_type =
    gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_INT);
  gcc_jit_type *v4si = gcc_jit_type_get_vector (int_type, 4);

  gcc_jit_function *func =
    gcc_jit_context_new_function (ctxt, NULL,
				  GCC_JIT_FUNCTION_EXPORTED,
				  v4si,
				  "epu32",
				  0, NULL,
				  0);
  gcc_jit_block *block = gcc_jit_function_new_block (func, NULL);
  gcc_jit_lvalue *var1 = gcc_jit_function_new_local (func, NULL, v2df, "var1");
  gcc_jit_lvalue *var2 = gcc_jit_function_new_local (func, NULL, v4si, "var2");
  gcc_jit_rvalue *args[3] = {
    gcc_jit_lvalue_as_rvalue (var1),
    gcc_jit_lvalue_as_rvalue (var2),
    gcc_jit_context_zero (ctxt, u8_type),
  };
  gcc_jit_rvalue *call = gcc_jit_context_new_call (ctxt, NULL, builtin, 3, args);
  gcc_jit_block_end_with_return (block, NULL, call);
}

void
verify_code (gcc_jit_context *ctxt, gcc_jit_result *result)
{
  CHECK_NON_NULL (result);
}

int
main (int argc, char **argv)
{
  /*  This is the same as the main provided by harness.h, but it first create a dummy context and compile
      in order to add the target builtins to libgccjit's internal state.  */
  gcc_jit_context *ctxt;
  ctxt = gcc_jit_context_acquire ();
  if (!ctxt)
    {
      fail ("gcc_jit_context_acquire failed");
      return -1;
    }
  gcc_jit_result *result;
  result = gcc_jit_context_compile (ctxt);
  gcc_jit_result_release (result);
  gcc_jit_context_release (ctxt);

  int i;

  for (i = 1; i <= 5; i++)
    {
      snprintf (test, sizeof (test),
		"%s iteration %d of %d",
                extract_progname (argv[0]),
                i, 5);

      //printf ("ITERATION %d\n", i);
      test_jit (argv[0], NULL);
      //printf ("\n");
    }

  totals ();

  return 0;
}
