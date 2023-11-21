/* Example of multiple locations, with labelling of ranges.

   Intended output is similar to:

PATH/test-labelled-ranges.c:9: error: mismatching types: 'int' and 'const char *'
   19 |   42 + "foo"
      |   ~~ ^ ~~~~~
      |   |    |
      |   int  const char *

   along with the equivalent in SARIF.  */

#include "libdiagnostics.h"
#include "test-helpers.h"

/*
_________11111111112
12345678901234567890
  42 + "foo"
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
  const diagnostic_physical_location *loc_operator
    = diagnostic_manager_new_location_from_file_line_column (diag_mgr, file, line_num, 6);


  diagnostic *d = diagnostic_begin (diag_mgr,
				    DIAGNOSTIC_LEVEL_ERROR);
  diagnostic_set_location (d, loc_operator);
  diagnostic_add_location_with_label (d,
				      make_range (diag_mgr, file, line_num, 3, 4),
				      "int");
  diagnostic_add_location_with_label (d,
				      make_range (diag_mgr, file, line_num, 8, 12),
				      "const char *");
  
  diagnostic_finish (d, "mismatching types: %qs and %qs", "int", "const char *");
  
  diagnostic_manager_release (diag_mgr);
  return 0;
}
