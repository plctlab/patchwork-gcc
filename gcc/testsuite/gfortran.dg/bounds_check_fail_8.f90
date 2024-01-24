! { dg-do compile }
! { dg-additional-options "-fcheck=bounds -g -fdump-tree-original" }
! { dg-output "At line 22 .*" }
! { dg-shouldfail "dimension 3 of array 'uu%z' outside of expected range" }
!
! PR fortran/30802 - improve bounds-checking for array references
!
! Checking the proper component references is the most important part here.

program test
  implicit none
  integer :: k = 0
  type t
     real, dimension(10,20,30) :: z = 23
  end type t
  type u
     type(t) :: vv(4,5)
  end type u
  type(t) :: uu,     ww(1)
  type(u) :: x1, x2, y1(1), y2(1)

  print *, uu   % z(1,k,:)           ! runtime check only for dimension 2 of z
  print *, ww(1)% z(1,:,k)           ! runtime check only for dimension 3 of z
  print *, x1   % vv(2,3)% z(1,:,k)  ! runtime check only for dimension 3 of z
  print *, x2   % vv(k,:)% z(1,2,3)  ! runtime check only for dimension 1 of vv
  print *, y1(1)% vv(2,3)% z(1,:,k)  ! runtime check only for dimension 3 of z
  print *, y2(1)% vv(:,k)% z(1,2,3)  ! runtime check only for dimension 2 of vv
end program test

! { dg-final { scan-tree-dump-times "'uu%%z.' outside of expected range" 2 "original" } }
! { dg-final { scan-tree-dump-times "'ww%%z.' outside of expected range" 2 "original" } }
! { dg-final { scan-tree-dump-times "'x1%%vv%%z.' outside of expected range" 2 "original" } }
! { dg-final { scan-tree-dump-times "'x2%%vv.' outside of expected range" 2 "original" } }
! { dg-final { scan-tree-dump-times "'y1%%vv%%z.' outside of expected range" 2 "original" } }
! { dg-final { scan-tree-dump-times "'y2%%vv.' outside of expected range" 2 "original" } }
