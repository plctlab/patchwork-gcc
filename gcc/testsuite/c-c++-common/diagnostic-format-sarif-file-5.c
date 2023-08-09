/* The goal is to test SARIF output of generated data, such as a _Pragma string.
   But SARIF output as of yet does not output macro definitions, so such
   generated data buffers never end up in the typical SARIF output.  One way we
   can achieve it is to use -fdump-internal-locations, which outputs top-level
   diagnostic notes inside macro definitions, that SARIF will end up processing.
   It also outputs a lot of other stuff to stderr (not to the SARIF file) that
   is not relevant to this test, so we use a blanket dg-regexp to filter all of
   that away.  */

/* { dg-do compile } */
/* { dg-options "-fdiagnostics-format=sarif-file -fdump-internal-locations" } */
/* { dg-allow-blank-lines-in-output "" } */

_Pragma("GCC diagnostic push")

/* { dg-regexp {(.|[\n\r])*} } */

/* Because of the way -fdump-internal-locations works, these regexes themselves
   will end up in the sarif output also.  But due to the escaping, they don't
   match themselves, so they still test what we need.  */

/* Four of this pair are output for the tokens inside the
   _Pragma string (3 plus a PRAGMA_EOL).  */

/* { dg-final { scan-sarif-file "\"artifactLocation\": \{\"uri\": \"<generated>\"," } } */
/* { dg-final { scan-sarif-file "\"snippet\": \{\"text\": \"GCC diagnostic push\\\\n\"" } } */

/* One of this pair is output for the overall internal location.  */

/* { dg-final { scan-sarif-file "\{\"location\": \{\"uri\": \"<generated>\"," } } */
/* { dg-final { scan-sarif-file "\"contents\": \{\"text\": \"GCC diagnostic push\\\\n\\\\0" } } */
