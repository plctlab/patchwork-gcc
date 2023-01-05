/* Evaluating the _Pragma directive here creates an LC_GEN map in the
   line_maps object that will be stored in the PCH.  The test will make sure
   that the buffer holding the de-stringified _Pragma string contents makes
   its way there.  */
_Pragma("this string should end up in the \"PCH\"")
