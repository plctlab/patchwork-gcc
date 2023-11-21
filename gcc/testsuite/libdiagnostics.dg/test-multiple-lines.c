/* Example of a warning with multiple locations in various source lines,
   with an insertion fix-it hint.

   Intended output is similar to:
   
/PATH/test-multiple-lines.c:17: warning: missing comma
   16 | const char *strs[3] = {"foo",
      |                        ~~~~~ 
   17 |                        "bar"
      |                        ~~~~~^
   18 |                        "baz"};
      |                        ~~~~~ 

   along with the equivalent in SARIF.  */

#include "libdiagnostics.h"
#include "test-helpers.h"

/* Placeholder source (missing comma after "bar"):
_________11111111112222222222
12345678901234567890123456789
const char *strs[3] = {"foo",
                       "bar"
                       "baz"};
*/
const int foo_line_num = __LINE__ - 4;

int
main ()
{
  diagnostic_manager *diag_mgr = diagnostic_manager_new ();

  diagnostic_manager_add_text_sink (diag_mgr, stderr,
				    DIAGNOSTIC_COLORIZE_IF_TTY);
  diagnostic_manager_add_sarif_sink (diag_mgr, stderr,
				     DIAGNOSTIC_SARIF_VERSION_2_1_0);

  const diagnostic_file *file = diagnostic_manager_new_file (diag_mgr, __FILE__, "c");
  const diagnostic_physical_location *loc_comma
    = diagnostic_manager_new_location_from_file_line_column (diag_mgr, file, foo_line_num + 1, 29);
  const diagnostic_physical_location *loc_foo
    = make_range (diag_mgr, file, foo_line_num, 24, 28);
  const diagnostic_physical_location *loc_bar
    = make_range (diag_mgr, file, foo_line_num + 1, 24, 28);
  const diagnostic_physical_location *loc_baz
    = make_range (diag_mgr, file, foo_line_num + 2, 24, 28);

  diagnostic *d = diagnostic_begin (diag_mgr,
				    DIAGNOSTIC_LEVEL_WARNING);
  diagnostic_set_location (d, loc_comma);
  diagnostic_add_location (d, loc_foo);
  diagnostic_add_location (d, loc_bar);
  diagnostic_add_location (d, loc_baz);

  diagnostic_add_fix_it_hint_insert_after (d, loc_bar, ",");
  
  diagnostic_finish (d, "missing comma");
  
  diagnostic_manager_release (diag_mgr);
  return 0;
};
