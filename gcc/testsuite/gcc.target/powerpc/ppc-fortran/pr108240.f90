! { dg-options "-mmodulo -mcpu=401" }
! This need one explicit 64 bit option on 64 bit environment
! to avoid possible error or warning message.
! { dg-additional-options "-m64" { target lp64 } }

! Verify there is no ICE on 64 bit environment.

program main
  implicit none
  integer, parameter :: n=4
  character(len=4), dimension(n,n) :: c
  integer, dimension(n,n) :: a
  integer, dimension(2) :: res1, res2
  real, dimension(n,n) :: r
  integer :: i,j
  character(len=4,kind=4), dimension(n,n) :: c4

  call random_number (r)
  a = int(r*100)

  do j=1,n
     do i=1,n
        write (*,*) a(i,j)
     end do
  end do

end program main
