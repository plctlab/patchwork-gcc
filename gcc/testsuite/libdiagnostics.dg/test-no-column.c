/* Example of emitting an error without a column number.

   Intended output is similar to:

PATH/test-error-with-note.c:6: error: can't find 'foo'
    6 | PRINT "hello world!";

   along with the equivalent in SARIF.  */

#include "libdiagnostics.h"

/*
_________111111111122
123456789012345678901
PRINT "hello world!";
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
  const diagnostic_physical_location *loc
    = diagnostic_manager_new_location_from_file_and_line (diag_mgr, file, line_num);

  diagnostic *d = diagnostic_begin (diag_mgr,
				    DIAGNOSTIC_LEVEL_ERROR);
  diagnostic_set_location (d, loc);
  
  diagnostic_finish (d, "can't find %qs", "foo");

  diagnostic_manager_release (diag_mgr);
  return 0;
};
