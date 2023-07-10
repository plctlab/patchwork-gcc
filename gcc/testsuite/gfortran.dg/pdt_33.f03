! { dg-do run }
!
! Test the fix for PR102003, where len parameters where not returned as constants.
!
! Contributed by Harald Anlauf  <anlauf@gcc.gnu.org>
!
program pr102003
  type pdt(n)
     integer, len     :: n = 8
     character(len=n) :: c
  end type pdt
  type(pdt(42)) :: p
  integer, parameter :: m = len (p% c)

  if (m /= 42) stop 1
  if (len (p% c) /= 42) stop 2
end

