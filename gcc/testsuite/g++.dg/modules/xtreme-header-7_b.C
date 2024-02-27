// A version of xtreme-header_{a.H,b.C} that doesn't pass
// -fno-module-lazy.
// { dg-additional-options -fmodules-ts }

#include "xtreme-header.h"
import "xtreme-header-7_a.H";
