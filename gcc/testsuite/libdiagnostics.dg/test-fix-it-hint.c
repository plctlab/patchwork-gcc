/* Example of a fix-it hint, including patch generation.

   Intended output is similar to:

PATH/test-fix-it-hint.c:19: error: unknown field 'colour'; did you mean 'color'
   19 |   return p->colour;
      |             ^~~~~~
      |             color

   along with the equivalent in SARIF, and a generated patch (on stderr) to
   make the change.  */

#include "libdiagnostics.h"
#include "test-helpers.h"

/*
_________11111111112
12345678901234567890
  return p->colour;
*/
const int line_num = __LINE__ - 2;

int
main ()
{
  diagnostic_manager *diag_mgr = diagnostic_manager_new ();

  diagnostic_manager_add_text_sink (diag_mgr, stderr,
				    DIAGNOSTIC_COLORIZE_IF_TTY);
  diagnostic_manager_add_sarif_sink (diag_mgr, stderr,
				     DIAGNOSTIC_SARIF_VERSION_2_1_0);

  const diagnostic_file *file = diagnostic_manager_new_file (diag_mgr, __FILE__, "c");
  const diagnostic_physical_location *loc_token
    = make_range (diag_mgr, file, line_num, 13, 18);

  diagnostic *d = diagnostic_begin (diag_mgr,
				    DIAGNOSTIC_LEVEL_ERROR);
  diagnostic_set_location (d, loc_token);

  diagnostic_add_fix_it_hint_replace (d, loc_token, "color");
  
  diagnostic_finish (d, "unknown field %qs; did you mean %qs", "colour", "color");

  diagnostic_manager_write_patch (diag_mgr, stderr);

  diagnostic_manager_release (diag_mgr);
  return 0;
}
