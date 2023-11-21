/* Example of a grouped error and note, with a fix-it hint on the note.

   Intended output is similar to:
   
/PATH/test-note-with-fix-it-hint.c:19: error: unknown field 'colour'
   19 |   return p->colour;
      |             ^~~~~~
/PATH/test-note-with-fix-it-hint.c:19: note: did you mean 'color'
   19 |   return p->colour;
      |             ^~~~~~
      |             color

   along with the equivalent in SARIF.  */

#include "libdiagnostics.h"
#include "test-helpers.h"

/* Placeholder source:
_________11111111112
12345678901234567890
  return p->colour;
*/
const int line_num = __LINE__ - 2;

int
main ()
{
  diagnostic_manager *diag_mgr = diagnostic_manager_new ();

  diagnostic_manager_add_text_sink (diag_mgr, stderr, DIAGNOSTIC_COLORIZE_IF_TTY);
  diagnostic_manager_add_sarif_sink (diag_mgr, stderr, DIAGNOSTIC_SARIF_VERSION_2_1_0);

  const diagnostic_file *file = diagnostic_manager_new_file (diag_mgr, __FILE__, "c");
  const diagnostic_physical_location *loc_token
    = make_range (diag_mgr, file, line_num, 13, 18);

  diagnostic_manager_begin_group (diag_mgr);

  diagnostic *err = diagnostic_begin (diag_mgr, DIAGNOSTIC_LEVEL_ERROR);
  diagnostic_set_location (err, loc_token);
  diagnostic_finish (err, "unknown field %qs", "colour");

  diagnostic *n = diagnostic_begin (diag_mgr, DIAGNOSTIC_LEVEL_NOTE);
  diagnostic_set_location (n, loc_token);
  diagnostic_add_fix_it_hint_replace (n, loc_token, "color");
  diagnostic_finish (n, "did you mean %qs", "color");

  diagnostic_manager_end_group (diag_mgr);

  diagnostic_manager_release (diag_mgr);
  return 0;
}
