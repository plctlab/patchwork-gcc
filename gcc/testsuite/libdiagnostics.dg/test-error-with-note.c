/* Example of emitting an error with an associated note.

   Intended output is similar to:

PATH/test-error-with-note.c:6: error: can't find 'foo'
    6 | PRINT "hello world!";
      |        ^~~~~~~~~~~~
PATH/test-error-with-note.c:6: note: have you looked behind the couch?

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
  const diagnostic_physical_location *loc_start
    = diagnostic_manager_new_location_from_file_line_column (diag_mgr, file, line_num, 8);
  const diagnostic_physical_location *loc_end
    = diagnostic_manager_new_location_from_file_line_column (diag_mgr, file, line_num, 19);
  const diagnostic_physical_location *loc_range
    = diagnostic_manager_new_location_from_range (diag_mgr,
						  loc_start,
						  loc_start,
						  loc_end);

  diagnostic_manager_begin_group (diag_mgr);
  
  diagnostic *err = diagnostic_begin (diag_mgr,
				      DIAGNOSTIC_LEVEL_ERROR);
  diagnostic_set_location (err, loc_range);
  diagnostic_finish (err, "can't find %qs", "foo");

  diagnostic *note = diagnostic_begin (diag_mgr, DIAGNOSTIC_LEVEL_NOTE);
  diagnostic_set_location (note, loc_range);
  diagnostic_finish (note, "have you looked behind the couch?");

  diagnostic_manager_end_group (diag_mgr);

  diagnostic_manager_release (diag_mgr);
  return 0;
};
