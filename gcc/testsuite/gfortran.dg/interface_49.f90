! { dg-do run }
! PR fortran/61615 - resolve correct generic with TYPE(C_PTR) arguments
! Contributed by Jacob Abel

MODULE foo
  USE iso_c_binding, only : c_ptr
  IMPLICIT NONE
  integer :: rank = -99
  INTERFACE bar
    MODULE PROCEDURE bar_s
    MODULE PROCEDURE bar_a1d
  END INTERFACE bar
CONTAINS
  SUBROUTINE bar_s(a)
    TYPE(c_ptr) :: a
    WRITE (0, *) 'in bar_s'
    rank = 0
  END SUBROUTINE bar_s

  SUBROUTINE bar_a1d(a)
    TYPE(c_ptr) :: a(:)
    WRITE (0, *) 'in bar_a1d'
    rank = 1
  END SUBROUTINE bar_a1d
END MODULE foo

PROGRAM cptr_array_vs_scalar_arg
  USE foo
  USE iso_c_binding, only : c_ptr, c_loc
  IMPLICIT NONE
  INTEGER, TARGET :: i
  TYPE(c_ptr)     :: a, b(1)
  a    = C_LOC(i)
  b(1) = C_LOC(i)
  CALL bar(a)
  if (rank /= 0) stop 1
  CALL bar(b)
  if (rank /= 1) stop 2
  CALL bar((a))
  if (rank /= 0) stop 3
  CALL bar((b))
  if (rank /= 1) stop 4
END PROGRAM cptr_array_vs_scalar_arg
