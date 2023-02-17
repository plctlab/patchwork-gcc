! { dg-do run { target offload_target_any } }
! { dg-shouldfail "present error triggered" }

program main
  implicit none
  integer, parameter :: N = 100
  integer :: a(N), b(N), c(N), i

  do i = 1, N
    a(i) = i * 2
    b(i) = i * 3 + 1
  end do

  !$omp target enter data map (alloc: a, c)
    ! This should work as a has already been allocated.
    !$omp target update to (present: a)

    ! This should fail as b has not been allocated.
    ! { dg-output "libgomp: present clause: !omp_target_is_present \\\(0x\[0-9a-f\]+, \[0-9\]+\\\)" }
    !$omp target update to (present: b)
  !$omp target exit data map (from: c)
end program
