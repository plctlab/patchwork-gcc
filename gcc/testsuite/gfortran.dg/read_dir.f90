! { dg-do run }
! { dg-additional-options "-cpp" }
! { dg-additional-sources read_dir-aux.c }
!
! PR67367

program bug
   use iso_c_binding
   implicit none

   interface
     subroutine my_mkdir(s) bind(C)
       ! Call POSIX's mkdir - and ignore fails due to
       ! existing directories but fail otherwise
       import
       character(len=1,kind=c_char) :: s(*)
     end subroutine
     subroutine my_rmdir(s) bind(C)
       ! Call POSIX's rmdir - and ignore fails
       import
       character(len=1,kind=c_char) :: s(*)
     end subroutine
   end interface

   character(len=*), parameter :: sdir = "junko.dir"
   character(len=*,kind=c_char), parameter :: c_sdir = sdir // c_null_char

   character(len=1) :: c
   integer ios

   call my_mkdir(c_sdir)
   open(unit=10, file=sdir,iostat=ios,action='read',access='stream')

#if defined(__MINGW32__)
   ! Windows is documented to fail with EACCESS when trying to open a directory
   ! Note: Testing showed that __CYGWIN__ does permit opening directories
   call my_rmdir(c_sdir)
   if (ios == 0) &
      stop 3  ! Expected EACCESS
   stop 0  ! OK
#endif   

   if (ios.ne.0) then
      call my_rmdir(c_sdir)
      STOP 1
   end if
   read(10, iostat=ios) c
   if (ios.ne.21.and.ios.ne.0) then  ! EISDIR has often the value 21
      close(10, status='delete')
      STOP 2
   end if
   close(10, status='delete')
end program bug
