#include <AMReX_winstd.H>

#include <NavierStokesBase.H>
#include <AMReX_BLFort.H>
#include <PROB_NS_F.H>

//
// Virtual access function for getting the forcing terms for the
// velocities and scalars.  The base version computes a buoyancy.
//
// As NavierStokesBase is currently implemented.  Velocities are integrated
// according to the equation
//
//     ui_t + uj ui_j = S_ui        ===> tforces = rho S_ui
//
// and scalars psi where (psi = rho q) as
//
//     psi_t + (uj psi)_j = S_psi   ===> tforces = S_psi = rho S_q
//
// q is a concentration.  This function returns a rho weighted
// source term, which requires a division by rho in the predict_velocity
// and velocity_advection routines.
//

#ifdef BOUSSINESQ
void
NavierStokesBase::getForce (FArrayBox&       force,
			    int              gridno,
			    int              ngrow,
			    int              scomp,
			    int              ncomp,
			    const Real       time,
			    const FArrayBox& Scal)
{
    force.resize(BoxLib::grow(grids[gridno],ngrow),ncomp);

    if (scomp == Xvel && ncomp == BL_SPACEDIM)
    {
       if (Scal.nComp() > 1) 
       {
          std::cout << "OOPS -- ONLY SUPPOSED TO BE ONE COMPONENT IN SCALAR " << std::endl;
          exit(0);
       }

       const Real* dx       = geom.CellSize();
       const int*  f_lo     = force.loVect();
       const int*  f_hi     = force.hiVect();
       const int*  s_lo     = Scal.loVect();
       const int*  s_hi     = Scal.hiVect();
   
       RealBox gridloc = RealBox(grids[gridno],geom.CellSize(),geom.ProbLo());

       const Real* ScalDataPtr = Scal.dataPtr(0);
       FORT_MAKEFORCE (force.dataPtr(), ScalDataPtr,
   		       ARLIM(f_lo), ARLIM(f_hi),
   		       ARLIM(s_lo), ARLIM(s_hi),
   		       dx, gridloc.lo(), gridloc.hi(),
   		       &scomp,&ncomp);
    } else {
       force.setVal(0.0);
    }
}
#else
#ifdef GENGETFORCE
void
NavierStokesBase::getForce (FArrayBox&       force,
			    int              gridno,
			    int              ngrow,
			    int              scomp,
			    int              ncomp,
			    const Real       time,
			    const FArrayBox& Rho,
			    int              RComp)
{
    BL_ASSERT(Rho.nComp() == 1);

    force.resize(BoxLib::grow(grids[gridno],ngrow),ncomp);

    BL_ASSERT(Rho.box().contains(force.box()));

    RealBox     gridloc = RealBox(grids[gridno],geom.CellSize(),geom.ProbLo());
    const Real* dx      = geom.CellSize();
    const Real  grav    = gravity;
    const int*  s_lo    = Rho.loVect();
    const int*  s_hi    = Rho.hiVect();
    const int*  f_lo    = force.loVect();
    const int*  f_hi    = force.hiVect();

    FORT_MAKEFORCE (&time,
		    force.dataPtr(),
		    Rho.dataPtr(RComp),
		    ARLIM(f_lo), ARLIM(f_hi),
		    ARLIM(s_lo), ARLIM(s_hi),
		    dx,
		    gridloc.lo(),
		    gridloc.hi(),
		    &grav,&scomp,&ncomp);
}

#else
#ifdef MOREGENGETFORCE
void
NavierStokesBase::getForce (FArrayBox&       force,
			    int              gridno,
			    int              ngrow,
			    int              scomp,
			    int              ncomp,
			    const Real       time,
			    const FArrayBox& Vel,
			    const FArrayBox& Scal,
			    int              scalScomp)
{
    if (ParallelDescriptor::IOProcessor() && getForceVerbose) {
	std::cout << "NavierStokesBase::getForce(): Entered..." << std::endl 
		  << "time      = " << time << std::endl
		  << "scomp     = " << scomp << std::endl
		  << "ncomp     = " << ncomp << std::endl
		  << "ngrow     = " << ngrow << std::endl
		  << "scalScomp = " << scalScomp << std::endl;

	if (scomp==0)
	    if  (ncomp==3) std::cout << "Doing velocities only" << std::endl;
	    else           std::cout << "Doing all components" << std::endl;
	else if (scomp==3)
	    if  (ncomp==1) std::cout << "Doing density only" << std::endl;
	    else           std::cout << "Doing all scalars" << std::endl;
	else if (scomp==4) std::cout << "Doing tracer only" << std::endl;
	else               std::cout << "Doing individual scalar" << std::endl;

    }

    force.resize(BoxLib::grow(grids[gridno],ngrow),ncomp);

    const Real* VelDataPtr  = Vel.dataPtr();
    const Real* ScalDataPtr = Scal.dataPtr(scalScomp);

    const Real* dx       = geom.CellSize();
    const Real  grav     = gravity;
    const int*  f_lo     = force.loVect();
    const int*  f_hi     = force.hiVect();
    const int*  v_lo     = Vel.loVect();
    const int*  v_hi     = Vel.hiVect();
    const int*  s_lo     = Scal.loVect();
    const int*  s_hi     = Scal.hiVect();
    const int   nscal    = NUM_SCALARS;

    if (ParallelDescriptor::IOProcessor() && getForceVerbose) {
#if (BL_SPACEDIM == 3)
	std::cout << "NavierStokesBase::getForce(): Force Domain:" << std::endl;
	std::cout << "(" << f_lo[0] << "," << f_lo[1] << "," << f_lo[2] << ") - "
		  << "(" << f_hi[0] << "," << f_hi[1] << "," << f_hi[2] << ")" << std::endl;
	std::cout << "NavierStokesBase::getForce(): Vel Domain:" << std::endl;
	std::cout << "(" << v_lo[0] << "," << v_lo[1] << "," << v_lo[2] << ") - "
		  << "(" << v_hi[0] << "," << v_hi[1] << "," << v_hi[2] << ")" << std::endl;
	std::cout << "NavierStokesBase::getForce(): Scal Domain:" << std::endl;
	std::cout << "(" << s_lo[0] << "," << s_lo[1] << "," << s_lo[2] << ") - "
		  << "(" << s_hi[0] << "," << s_hi[1] << "," << s_hi[2] << ")" << std::endl;
#else
	std::cout << "NavierStokesBase::getForce(): Force Domain:" << std::endl;
	std::cout << "(" << f_lo[0] << "," << f_lo[1] << ") - "
		  << "(" << f_hi[0] << "," << f_hi[1] << ")" << std::endl;
	std::cout << "NavierStokesBase::getForce(): Vel Domain:" << std::endl;
	std::cout << "(" << v_lo[0] << "," << v_lo[1] << ") - "
		  << "(" << v_hi[0] << "," << v_hi[1] << ")" << std::endl;
	std::cout << "NavierStokesBase::getForce(): Scal Domain:" << std::endl;
	std::cout << "(" << s_lo[0] << "," << s_lo[1] << ") - "
		  << "(" << s_hi[0] << "," << s_hi[1] << ")" << std::endl;
#endif

	Array<Real> velmin(BL_SPACEDIM), velmax(BL_SPACEDIM);
	Array<Real> scalmin(NUM_SCALARS), scalmax(NUM_SCALARS);
	for (int n=0; n<BL_SPACEDIM; n++) {
	    velmin[n]= 1.e234;
	    velmax[n]=-1.e234;
	}
	int ix = v_hi[0]-v_lo[0]+1;
	int jx = v_hi[1]-v_lo[1]+1;
#if (BL_SPACEDIM == 3)
	int kx = v_hi[2]-v_lo[2]+1;
	for (int k=0; k<kx; k++) {
#endif
	    for (int j=0; j<jx; j++) {
		for (int i=0; i<ix; i++) {
		    for (int n=0; n<BL_SPACEDIM; n++) {
#if (BL_SPACEDIM == 3)
			int cell = ((n*kx+k)*jx+j)*ix+i;
#else
			int cell = (n*jx+j)*ix+i;
#endif
			Real v = VelDataPtr[cell];
			if (v<velmin[n]) velmin[n] = v;
			if (v>velmax[n]) velmax[n] = v;
		    }
		}
	    }
#if (BL_SPACEDIM == 3)
	}
#endif
	for (int n=0; n<BL_SPACEDIM; n++) 
	    std::cout << "Vel  " << n << " min/max " << velmin[n] << " / " << velmax[n] << std::endl;
	
	for (int n=0; n<NUM_SCALARS; n++) {
	    scalmin[n]= 1.e234;
	    scalmax[n]=-1.e234;
	}
	ix = s_hi[0]-s_lo[0]+1;
	jx = s_hi[1]-s_lo[1]+1;
#if (BL_SPACEDIM == 3)
	kx = s_hi[2]-s_lo[2]+1;
	for (int k=0; k<kx; k++) {
#endif
	    for (int j=0; j<jx; j++) {
		for (int i=0; i<ix; i++) {
		    for (int n=0; n<NUM_SCALARS; n++) {
#if (BL_SPACEDIM == 3)
			int cell = ((n*kx+k)*jx+j)*ix+i;
#else
			int cell = (n*jx+j)*ix+i;
#endif
			Real s = ScalDataPtr[cell];
			if (s<scalmin[n]) scalmin[n] = s;
			if (s>scalmax[n]) scalmax[n] = s;
		    }
		}
	    }
#if (BL_SPACEDIM == 3)
	}
#endif
	for (int n=0; n<NUM_SCALARS; n++) 
	    std::cout << "Scal " << n << " min/max " << scalmin[n] << " / " << scalmax[n] << std::endl;
    }

    RealBox gridloc = RealBox(grids[gridno],geom.CellSize(),geom.ProbLo());
    
    // Here's the meat
    FORT_MAKEFORCE (&time,
		    force.dataPtr(),
		    VelDataPtr,
		    ScalDataPtr,
		    ARLIM(f_lo), ARLIM(f_hi),
		    ARLIM(v_lo), ARLIM(v_hi),
		    ARLIM(s_lo), ARLIM(s_hi),
		    dx,
		    gridloc.lo(),
		    gridloc.hi(),
		    &grav,&scomp,&ncomp,&nscal,&getForceVerbose);

    if (ParallelDescriptor::IOProcessor() && getForceVerbose) {
	Array<Real> forcemin(ncomp);
	Array<Real> forcemax(ncomp);
	for (int n=0; n<ncomp; n++) {
	    forcemin[n]= 1.e234;
	    forcemax[n]=-1.e234;
	}
	int ix = f_hi[0]-f_lo[0]+1;
	int jx = f_hi[1]-f_lo[1]+1;
#if (BL_SPACEDIM == 3)
	int kx = f_hi[2]-f_lo[2]+1;
	for (int k=0; k<kx; k++) {
#endif
	    for (int j=0; j<jx; j++) {
		for (int i=0; i<ix; i++) {
		    for (int n=0; n<ncomp; n++) {
#if (BL_SPACEDIM == 3)
			int cell = ((n*kx+k)*jx+j)*ix+i;
#else
			int cell = (n*jx+j)*ix+i;
#endif
			Real f = force.dataPtr()[cell];
			if (f<forcemin[n]) forcemin[n] = f;
			if (f>forcemax[n]) forcemax[n] = f;
		    }
		}
	    }
#if (BL_SPACEDIM == 3)
	}
#endif
	for (int n=0; n<ncomp; n++) 
	    std::cout << "Force " << n+scomp << " min/max " << forcemin[n] << " / " << forcemax[n] << std::endl;
	
	std::cout << "NavierStokesBase::getForce(): Leaving..." << std::endl << "---" << std::endl;
    }
}
#else
void
NavierStokesBase::getForce (FArrayBox&       force,
			    int              gridno,
			    int              ngrow,
			    int              scomp,
			    int              ncomp,
			    const FArrayBox& Rho,
			    int              RComp)
{
    BL_ASSERT(Rho.nComp() > RComp);

    force.resize(BoxLib::grow(grids[gridno],ngrow),ncomp);

    BL_ASSERT(Rho.box().contains(force.box()));

    const Real grav = gravity;

    for (int dc = 0; dc < ncomp; dc++)
    {
        const int sc = scomp + dc;
#if (BL_SPACEDIM == 2)
        if (sc == Yvel && std::fabs(grav) > 0.001) 
#endif
#if (BL_SPACEDIM == 3)
        if (sc == Zvel && std::fabs(grav) > 0.001) 
#endif
        {
            //
            // Set force to -rho*g.
            //
     	    force.copy(Rho,RComp,dc,1);
            force.mult(grav,dc,1);
        }
        else
        {
            force.setVal(0,dc);
        }
    }    
}
#endif
#endif
#endif
