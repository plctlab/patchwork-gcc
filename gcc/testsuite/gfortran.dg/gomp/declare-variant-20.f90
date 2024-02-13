! { dg-additional-options "-fdump-tree-gimple-asmname" }

! This tests that mangled names, i.e. DECL_NAME != DECL_ASSEMBLER_NAME
! are properly handled

! This test case failed before with:
!   undefined reference to `foo'
! as the actual symbol is __m_MOD_foo

! NOTE 1: This test relies  on late resolution of condition,
! which is here enforced via the always_false_flag variable.
!
! NOTE 2: Using a variable is an OpenMP 5.1 feature that is/was not supported
! when this test case was created, cf. PR middle-end/113904

module m
  implicit none (type, external)
  logical :: always_false_flag = .false.
contains
  integer function variant1() result(res)
    res = 1
  end function

  integer function variant2() result(res)
    res = 2
  end function

  integer function variant3() result(res)
    res = 3
  end function

  integer function foo() result(res)
    !$omp  declare variant(variant1) match(construct={teams})
    !$omp  declare variant(variant2) match(construct={parallel})
    !$omp  declare variant(variant3) match(user={condition(always_false_flag)},construct={target})
    res = 99
  end
end module m

program main
  use m
  implicit none (type, external)
  integer :: r1, r2, r3

  r1 = foo()
  if (r1 /= 99) stop 1

  !$omp parallel if (.false.)
    r2 = foo()
    if (r2 /= 2) stop 2
  !$omp end parallel

  !$omp teams num_teams(1)
    r3 = foo()
    if (r3 /= 1) stop 3
  !$omp end teams

end program 

! { dg-final { scan-tree-dump-times "r1 = __m_MOD_foo \\(\\);" 1 "gimple" } }
! { dg-final { scan-tree-dump-times "r2 = __m_MOD_variant2 \\(\\);" 1 "gimple" } }
! { dg-final { scan-tree-dump-times "r3 = __m_MOD_variant1 \\(\\);" 1 "gimple" } }
