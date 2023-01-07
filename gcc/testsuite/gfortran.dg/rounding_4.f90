! { dg-do run }
module y
  implicit none
  integer, parameter :: wp = selected_real_kind(15)
contains
  subroutine foo(a,b,c)
    use ieee_arithmetic
    real(kind=wp), dimension(4), intent(out) :: a
    real(kind=wp), intent(in) :: b, c
    type (ieee_round_type), dimension(4), parameter :: mode = &
         [ieee_nearest, ieee_to_zero, ieee_up, ieee_down]
    call ieee_set_rounding_mode (mode(1))
    a(1) = b + c
    call ieee_set_rounding_mode (mode(2))
    a(2) = b + c
    call ieee_set_rounding_mode (mode(3))
    a(3) = b + c
    call ieee_set_rounding_mode (mode(4))
    a(4) = b + c
  end subroutine foo
end module y

program main
  use y
  real(kind=wp), dimension(4) :: a
  call foo(a,0.1_wp,0.2_wp)
  if (a(1) <= a(2)) stop 1
  if (a(3) <= a(4)) stop 2
  if (a(1) /= a(3)) stop 3
  if (a(2) /= a(4)) stop 4
end program main
