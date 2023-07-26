! Copyright 2009 Free Software Foundation, Inc.

! This program is free software; you can redistribute it and/or modify
! it under the terms of the GNU General Public License as published by
! the Free Software Foundation; either version 3 of the License, or
! (at your option) any later version.
!
! This program is distributed in the hope that it will be useful,
! but WITHOUT ANY WARRANTY; without even the implied warranty of
! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
! GNU General Public License for more details.
!
! You should have received a copy of the GNU General Public License
! along with this program.  If not, see <http://www.gnu.org/licenses/>.

      use omp_lib
      integer nthreads, i, a(1000)
      nthreads = omp_get_num_threads()
      if (nthreads .gt. 1000) call abort

      do i = 1, nthreads
          a(i) = 0
      end do
      print *, "start-here"
!$omp parallel
      a(omp_get_thread_num() + 1) = 1
!$omp end parallel
      do i = 1, nthreads
          if (a(i) .ne. 1) call abort
      end do
      print *, "success"
      end
