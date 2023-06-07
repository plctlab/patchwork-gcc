// PR c++/33492
// { dg-require-effective-target exceptions_enabled }
// { dg-options "" }

void foo()
{
  if (throw 0) // { dg-error "could not convert .\\<throw-expression\\>. from .void. to .bool." }
    ;
}
