/* Example of setting a CWE and adding extra metadata.

   Intended output is similar to:

PATH/test-metadata.c:21: warning: never use 'gets' [CWE-242] [STR34-C]
   21 |   gets (buf);
      |   ^~~~~~~~~~

   where the metadata tags are linkified in a sufficiently capable terminal,
   along with the equivalent in SARIF.  */

#include "libdiagnostics.h"
#include "test-helpers.h"

/* Placeholder source:
_________11111111112
12345678901234567890
void test_cwe (void)
{
  char buf[1024];
  gets (buf);
}
*/
const int line_num = __LINE__ - 3;

int
main ()
{
  diagnostic_manager *diag_mgr = diagnostic_manager_new ();

  diagnostic_manager_set_tool_name (diag_mgr, "FooChecker");
  diagnostic_manager_set_full_name (diag_mgr, "FooChecker 0.1 (en_US)");
  diagnostic_manager_set_version_string (diag_mgr, "0.1");
  diagnostic_manager_set_version_url (diag_mgr, "https://www.example.com/0.1/");

  diagnostic_manager_add_text_sink (diag_mgr, stderr,
				    DIAGNOSTIC_COLORIZE_IF_TTY);
  diagnostic_manager_add_sarif_sink (diag_mgr, stderr,
				     DIAGNOSTIC_SARIF_VERSION_2_1_0);

  const diagnostic_file *file = diagnostic_manager_new_file (diag_mgr, __FILE__, "c");
  const diagnostic_physical_location *loc_token
    = make_range (diag_mgr, file, line_num, 3, 12);
  diagnostic *d = diagnostic_begin (diag_mgr,
				    DIAGNOSTIC_LEVEL_WARNING);
  diagnostic_set_location (d, loc_token);
  diagnostic_set_cwe (d, 242); /* CWE-242: Use of Inherently Dangerous Function.  */
  diagnostic_add_rule (d, "STR34-C", "https://example.com/");
  
  diagnostic_finish (d, "never use %qs", "gets");

  diagnostic_manager_release (diag_mgr);
  return 0;
}
