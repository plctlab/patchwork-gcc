! PR fortran/107424

subroutine foo (av, avo, a0, a0o, a1, a2, a3, a4)
implicit none

integer, value :: av
integer, value :: avo
integer :: a0
integer :: a0o
integer, pointer :: a1
integer, pointer, optional :: a2
integer, allocatable :: a3
integer, allocatable, optional :: a4
integer :: a5
integer, pointer :: a6
integer, allocatable :: a7

integer :: j

!$omp simd collapse(2)
do av = 1, 10  ! { dg-message "sorry, unimplemented: non-rectangular loop nest with dummy-argument or pointer, optional or allocatable do-variable 'av'" }
  do j = av, 20  ! { dg-note "Used here" }
  end do
end do

!$omp simd collapse(2)
do avo = 1, 10  ! { dg-message "sorry, unimplemented: non-rectangular loop nest with dummy-argument or pointer, optional or allocatable do-variable 'avo'" }
  do j = avo, 20  ! { dg-note "Used here" }
  end do
end do

!$omp simd collapse(2)
do a0 = 1, 10  ! { dg-message "sorry, unimplemented: non-rectangular loop nest with dummy-argument or pointer, optional or allocatable do-variable 'a0'" }
  do j = a0, 20  ! { dg-note "Used here" }
  end do
end do

!$omp simd collapse(2)
do a0o = 1, 10  ! { dg-message "sorry, unimplemented: non-rectangular loop nest with dummy-argument or pointer, optional or allocatable do-variable 'a0o'" }
  do j = a0o, 20  ! { dg-note "Used here" }
  end do
end do

!$omp simd collapse(2)
do a1 = 1, 10  ! { dg-message "sorry, unimplemented: non-rectangular loop nest with dummy-argument or pointer, optional or allocatable do-variable 'a1'" }
  do j = a1, 20  ! { dg-note "Used here" }
  end do
end do

!$omp simd collapse(2)
do a2 = 1, 10  ! { dg-message "sorry, unimplemented: non-rectangular loop nest with dummy-argument or pointer, optional or allocatable do-variable 'a2'" }
  do j = a2, 20  ! { dg-note "Used here" }
  end do
end do

!$omp simd collapse(2)
do a3 = 1, 10  ! { dg-message "sorry, unimplemented: non-rectangular loop nest with dummy-argument or pointer, optional or allocatable do-variable 'a3'" }
  do j = a3, 20  ! { dg-note "Used here" }
  end do
end do

!$omp simd collapse(2)
do a4 = 1, 10  ! { dg-message "sorry, unimplemented: non-rectangular loop nest with dummy-argument or pointer, optional or allocatable do-variable 'a4'" }
  do j = a4, 20  ! { dg-note "Used here" }
  end do
end do

!$omp simd collapse(2)
do a5 = 1, 10
  do j = a5, 20
  end do
end do

!$omp simd collapse(2)
do a6 = 1, 10  ! { dg-message "sorry, unimplemented: non-rectangular loop nest with dummy-argument or pointer, optional or allocatable do-variable 'a6'" }
  do j = a6, 20  ! { dg-note "Used here" }
  end do
end do

!$omp simd collapse(2)
do a7 = 1, 10  ! { dg-message "sorry, unimplemented: non-rectangular loop nest with dummy-argument or pointer, optional or allocatable do-variable 'a7'" }
  do j = a7, 20  ! { dg-note "Used here" }
  end do
end do
end
