/* Common utility code shared between test cases.  */

#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

const diagnostic_physical_location *
make_range (diagnostic_manager *diag_mgr,
	    const diagnostic_file *file,
	    diagnostic_line_num_t line_num,
	    diagnostic_column_num_t start_column,
	    diagnostic_column_num_t end_column)
{
  const diagnostic_physical_location *loc_start
    = diagnostic_manager_new_location_from_file_line_column (diag_mgr,
							     file,
							     line_num,
							     start_column);
  const diagnostic_physical_location *loc_end
    = diagnostic_manager_new_location_from_file_line_column (diag_mgr,
							     file,
							     line_num,
							     end_column);
  return diagnostic_manager_new_location_from_range (diag_mgr,
						     loc_start,
						     loc_start,
						     loc_end);
}

#endif /* #ifndef TEST_HELPERS_H */
