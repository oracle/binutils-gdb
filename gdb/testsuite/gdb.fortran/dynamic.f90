! Copyright 2007 Free Software Foundation, Inc.
!
! This program is free software; you can redistribute it and/or modify
! it under the terms of the GNU General Public License as published by
! the Free Software Foundation; either version 2 of the License, or
! (at your option) any later version.
!
! This program is distributed in the hope that it will be useful,
! but WITHOUT ANY WARRANTY; without even the implied warranty of
! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
! GNU General Public License for more details.
!
! You should have received a copy of the GNU General Public License
! along with this program; if not, write to the Free Software
! Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
!
! Ihis file is the Fortran source file for dynamic.exp.
! Original file written by Jakub Jelinek <jakub@redhat.com>.
! Modified for the GDB testcase by Jan Kratochvil <jan.kratochvil@redhat.com>.

subroutine baz
  real, target, allocatable :: varx (:, :, :)
  real, pointer :: varv (:, :, :)
  real, target :: varu (1, 2, 3)
  logical :: l
  allocate (varx (1:6, 5:15, 17:28))            ! varx-init
  l = allocated (varx)
  varx(:, :, :) = 6                             ! varx-allocated
  varx(1, 5, 17) = 7
  varx(2, 6, 18) = 8
  varx(6, 15, 28) = 9
  varv => varx                                  ! varx-filled
  l = associated (varv)
  varv(3, 7, 19) = 10                           ! varv-associated
  varv => null ()                               ! varv-filled
  l = associated (varv)
  deallocate (varx)                             ! varv-deassociated
  l = allocated (varx)
  varu(:, :, :) = 10                            ! varx-deallocated
  allocate (varv (1:6, 5:15, 17:28))
  l = associated (varv)
  varv(:, :, :) = 6
  varv(1, 5, 17) = 7
  varv(2, 6, 18) = 8
  varv(6, 15, 28) = 9
  deallocate (varv)
  l = associated (varv)
  varv => varu
  varv(1, 1, 1) = 6
  varv(1, 2, 3) = 7
  l = associated (varv)
end subroutine baz
subroutine foo (vary, varw)
  real :: vary (:, :)
  real :: varw (:, :, :)
  vary(:, :) = 4                                ! vary-passed
  vary(1, 1) = 8
  vary(2, 2) = 9
  vary(1, 3) = 10
  varw(:, :, :) = 5                             ! vary-filled
  varw(1, 1, 1) = 6
  varw(2, 2, 2) = 7                             ! varw-almostfilled
end subroutine foo
subroutine bar (varz, vart)
  real :: varz (*)
  real :: vart (2:11, 7:*)
  varz(1:3) = 4
  varz(2) = 5                                   ! varz-almostfilled
  vart(2,7) = vart(2,7)
end subroutine bar
program test
  interface
    subroutine foo (vary, varw)
    real :: vary (:, :)
    real :: varw (:, :, :)
    end subroutine
  end interface
  interface
    subroutine bar (varz, vart)
    real :: varz (*)
    real :: vart (2:11, 7:*)
    end subroutine
  end interface
  real :: x (10, 10), y (5), z(8, 8, 8)
  x(:,:) = 1
  y(:) = 2
  z(:,:,:) = 3
  call baz
  call foo (x, z(2:6, 4:7, 6:8))
  call bar (y, x)
  if (x (1, 1) .ne. 8 .or. x (2, 2) .ne. 9 .or. x (1, 2) .ne. 4) call abort
  if (x (1, 3) .ne. 10) call abort
  if (z (2, 4, 6) .ne. 6 .or. z (3, 5, 7) .ne. 7 .or. z (2, 4, 7) .ne. 5) call abort
  if (any (y .ne. (/4, 5, 4, 2, 2/))) call abort
  call foo (transpose (x), z)
  if (x (1, 1) .ne. 8 .or. x (2, 2) .ne. 9 .or. x (1, 2) .ne. 4) call abort
  if (x (3, 1) .ne. 10) call abort
end
