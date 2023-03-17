! { dg-do compile }
! PR fortran/85877
! A procedure with the bind(c) attribute shall have an explicit interface
! Contributed by G. Steinmetz

subroutine p
  bind(c) f     ! { dg-error "must be explicit" }
  x = f()
end

subroutine s
  interface
     function g() bind(c)
     end function g
  end interface
  x = g()
end
