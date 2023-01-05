#include "LC_GEN-maps.H"

/* The LC_GEN map was written to the PCH, but there is not currently a way to
   observe that fact in normal user code.  Let's try to test it anyway, using
   -fdump-internal-locations to inspect the line_maps object we received from
   the PCH.  */

/* { dg-additional-options -fdump-internal-locations } */
/* { dg-allow-blank-lines-in-output "" } */

/* These regexps themselves will also appear in the output of
   -fdump-internal-locations, so we need to make sure they contain at least
   some regexp special characters, even if not strictly necessary, so they
   match the intended text only, and not themselves.  Also, we make the second
   one intentionally match the whole output if it maches anything.  We could
   use dg-excess-errors instead, but that outputs XFAILS which are not really
   helpful for this test.  */

/* { dg-regexp {reason: . \(LC_GEN\)} } */
/* { dg-regexp {(.|[\n\r])*data: this string should end up in the "PCH"(.|[\n\r])*} } */
