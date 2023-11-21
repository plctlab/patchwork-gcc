/* Usage example of dump API.  */

#include "libdiagnostics.h"

const int line_num = 42;

int
main ()
{
  diagnostic_manager *diag_mgr = diagnostic_manager_new ();

  const diagnostic_file *file = diagnostic_manager_new_file (diag_mgr, "foo.c", "c");

  fprintf (stderr, "file: ");
  diagnostic_manager_debug_dump_file (diag_mgr, file, stderr);
  fprintf (stderr, "\n");
  
  const diagnostic_physical_location *loc_start
    = diagnostic_manager_new_location_from_file_line_column (diag_mgr, file, line_num, 8);
  const diagnostic_physical_location *loc_end
    = diagnostic_manager_new_location_from_file_line_column (diag_mgr, file, line_num, 19);
  const diagnostic_physical_location *loc_range
    = diagnostic_manager_new_location_from_range (diag_mgr,
					  loc_start,
					  loc_start,
					  loc_end);

  fprintf (stderr, "loc_start: ");
  diagnostic_manager_debug_dump_location (diag_mgr, loc_start, stderr);
  fprintf (stderr, "\n");

  fprintf (stderr, "loc_end: ");
  diagnostic_manager_debug_dump_location (diag_mgr, loc_end, stderr);
  fprintf (stderr, "\n");

  fprintf (stderr, "loc_range: ");
  diagnostic_manager_debug_dump_location (diag_mgr, loc_range, stderr);
  fprintf (stderr, "\n");
  

  const diagnostic_logical_location *logical_loc
    = diagnostic_manager_new_logical_location (diag_mgr,
					       DIAGNOSTIC_LOGICAL_LOCATION_KIND_FUNCTION,
					       NULL, /* parent */
					       "test_short_name",
					       "test_qualified_name",
					       "test_decorated_name");

  fprintf (stderr, "logical_loc: ");
  diagnostic_manager_debug_dump_logical_location (diag_mgr, logical_loc, stderr);
  fprintf (stderr, "\n");
  
  diagnostic_manager_release (diag_mgr);
  return 0;
};
