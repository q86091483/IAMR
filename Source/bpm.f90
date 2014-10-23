module bpm
  use iso_c_binding
  implicit none
contains

  subroutine bpm_compute_forces_2d(x1, v1, f1, x2, v2, f2, sconstant, slength) bind(c)
    real(c_double), intent(in   )        :: x1(2), v1(2), x2(2), v2(2)
    real(c_double), intent(inout)        :: f1(2), f2(2)
    real(c_double), intent(in   ), value :: sconstant, slength

    real(c_double) :: force(2), delta(2), distance

    ! compute spring force
    delta    = x2 - x1
    distance = sqrt(delta(1)**2 + delta(2)**2)
    if (abs(distance - slength) > 1.d-12) then
       force = -sconstant * (distance - slength) * delta / distance
    else
       force = 0
    end if

    print *, "bpm:: x1:", x1
    print *, "bpm:: x2:", x2
    print *, "bpm:: f: ", force

    f1 = f1 + force
    f2 = f2 - force

  end subroutine bpm_compute_forces_2d

  subroutine bpm_spread_forces_2d(x1, force, diam, L, lo, hi, xlo, xhi, dx, f, divf) bind(c)
    real(c_double), intent(in   )        :: x1(2), force(2), xlo(2), xhi(2)
    real(c_double), intent(in   ), value :: diam, dx
    integer(c_int), intent(in   ), value :: L
    integer(c_int), intent(in   )        :: lo(2), hi(2)
    real(c_double), intent(inout)        :: f(lo(1):hi(1),lo(2):hi(2),2), divf(lo(1):hi(1),lo(2):hi(2))

    real(c_double), dimension(-L:L,-L:L) :: distx, disty, dist2, blob, dbdr

    real(c_double) :: a, b, x, y
    integer :: i, j, c, ij(2)

    ! compute cell that contains particle
    ij = nint((x1 - xlo) / dx + lo)! - 0.5d0)

    ! compute distances to cell centers
    do i = -L, L
       x = xlo(1) + dx * (ij(1) + i - lo(1) + 0.5d0)
       do j = -L, L
          y = xlo(2) + dx * (ij(2) + j - lo(2) + 0.5d0)
          distx(i,j) = x - x1(1)
          disty(i,j) = y - x1(2)
       end do
    end do

    dist2 = (distx**2 + disty**2) / diam**2

    ! compute blob and dbdr
    a =   6.0d0 / (3.1415926535897931d0 * diam**2)
    b = -60.0d0 / (3.1415926535897931d0 * diam**4)
    where (dist2 <= 1.0d0)
       blob = a * (1.0d0 - dist2)**5
       dbdr = b * (1.0d0 - dist2)**4
    elsewhere
       blob = 0
       dbdr = 0
    end where

    print *, "bpm:: blob volume", sum(blob)*dx**2
    do i = -L, L
       print *, blob(i,:)
    end do

    print *, "bpm:: dbdr"
    do i = -L, L
       print *, dbdr(i,:)
    end do


    ! spread force
    ! do c = 1, 2
    !    do i = -L, L
    !       do j = -L, L
    !          f(ij(1)+i,ij(2)+j,c) = f(ij(1)+i,ij(2)+j,c) + force(c) * blob(i,j)
    !       end do
    !    end do
    ! end do

    ! spread divergence
    ! do i = -L, L
    !    do j = -L, L
    !       divf(ij(1)+i,ij(2)+j) = divf(ij(1)+i,ij(2)+j) + &
    !            (force(1) * distx(i,j) + force(2) * disty(i,j)) * dbdr(i,j)
    !    end do
    ! end do

  end subroutine bpm_spread_forces_2d

end module bpm
