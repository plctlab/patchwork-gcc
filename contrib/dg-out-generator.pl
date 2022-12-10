#!/usr/bin/env perl
#
# Copyright (C) 2022 GCC Contributors.
# Contributed by Arsen Arsenović.
#
# This script is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.

# This script reads program output on STDIN, and out of it produces a block of
# dg-output lines that can be yanked at the end of a file.  It will escape
# special ARE and Tcl constructs automatically.
#
# Each argument passed on the standard input is treated as a string to be
# replaced by ``.*'' in the final result.  This is intended to mask out build
# paths, filenames, etc.
#
# Usage example:

# $ g++-13 -fcontracts -o test \
#  'g++.dg/contracts/contracts-access1.C' && \
#   ./test |& dg-out-generator.pl 'g++.dg/contracts/contracts-access1.C'
# // { dg-output "contract violation in function Base::b at .*:11: pub > 0(\n|\r\n|\r)*" }
# // { dg-output "\\\[level:default, role:default, continuation mode:never\\\](\n|\r\n|\r)*" }
# // { dg-output "terminate called without an active exception(\n|\r\n|\r)*" }
# You can now freely dump the above into your testcase.

use strict;
use warnings;
use POSIX 'floor';

my $escapees = '(' . join ('|', map { quotemeta } @ARGV) . ')';

sub gboundary($)
{
  my $str = shift;
  my $sz = 10.0;
  for (;;)
    {
      my $bnd = join '', (map chr 64 + rand 27, 1 .. floor $sz);
      return $bnd unless index ($str, $bnd) >= 0;
      $sz += 0.1;
    }
}

while (<STDIN>)
  {
    # Escape our escapees.
    my $boundary = gboundary $_;
    s/$escapees/$boundary/;

    # Quote stuff special in Tcl ARE.
    s/([[\]*+?{}()\\])/\\$1/g;

    # Then, special stuff in TCL itself.
    s/([\][\\])/\\$1/g;

    # Newlines should be more tolerant.
    s/\n$/(\\n|\\r\\n|\\r)*/;

    # Then split out the boundary, replacing it with .*.
    s/$boundary/.*/;

    # Then, let's print it in a dg-output block.
    print "// { dg-output \"$_\" }\n";
  }
