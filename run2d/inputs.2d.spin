# NOTE: all the values here must be set unless specifically
#       marked as optional

#**********************************
#PARALLEL CONTROLS (defaults shown; these are both optional)
#**********************************

nprocs = 1       # number of processors
#sleep  = 0

#**********************************
#RUN CONTROLS  
#**********************************

# MUST SET ONE OR BOTH OF THESE.  
max_step =        10  # maximum (integer) number of time steps
stop_time =       2.0 # maximum (real) time (in same units as dt)

amr.n_cell =      16 8 # indices of the domain at level 0

# (this one is optional)
#amr.restart =     # set if starting from a restart file, else comment out

# This will default to file "probin" if not set
amr.probin_file = probin.2d.spin

#**********************************
#ADAPTIVITY CONTROLS
#**********************************

amr.max_level      = 2 # maximum number of levels of refinement
amr.regrid_int     = 2 # how often to regrid if max_level > 0

# (this one is optional)
# amr.regrid_file      # file name for specifying fixed grids

# MUST SET ONLY ONE OF THE TWO BELOW (COMMENT OUT THE OTHER)
amr.ref_ratio        = 4 4 # integer refinement ratio 
#amr.ref_ratio_vect         # IntVect refinement ratio 

#**********************************
#VERBOSITY CONTROLS (these are all optional with defaults shown)
#**********************************

ns.v                    = 1
godunov.v               = 1
mac.v                   = 0
proj.v                  = 0
diffuse.v               = 0
amr.v                   = 1

mg.v                    = 1
cg.v                    = 0
proj.Pcode              = 0

#**********************************
#OUTPUT CONTROLS
#**********************************

#THESE FILES WILL BE CREATED ONLY IF YOU SPECIFY THE NAME
#amr.run_log          = runlog  # name of the run log file 
amr.grid_log          = grdlog  # name of the grid log file
#amr.data_log         = datalog # name of the data log file

#THESE FILES WILL BE CREATED WITH DEFAULT PREFIX IF YOU DONT SPECIFY OTHERWISE
#amr.check_file       # prefix of the checkpoint files (default = "chk")
#amr.plot_file        # prefix of the plot files       (default = "plt")

# (this one is optional - sums will only be done if this is set to positive)
#ns.sum_interval      = - 1# interval at which to sum the specified quantities

# MUST SET ONLY ONE OF THE TWO BELOW (COMMENT OUT THE OTHER)
#amr.check_per        # (real) period between checkpoint files (in same units as dt)
amr.check_int        = 20 # (integer) number of timesteps between checkpoint files

# MUST SET ONLY ONE OF THE TWO BELOW (COMMENT OUT THE OTHER)
#amr.plot_per         # (real) period between plot files (in same units as dt)
amr.plot_int         = 2 # (integer) number of timesteps between plot files

#**********************************
#TIME STEP CONTROLS
#**********************************

ns.cfl                  = 0.9  # CFL number used to set dt

# (this one is optional)
#ns.init_shrink          = 1.0  # factor which multiplies the very first time step

#**********************************
#PHYSICS
#**********************************

ns.vel_visc_coef        = 0.01 
ns.temp_cond_coef       = 0.0 
ns.scal_diff_coefs      = 0.01

# (this one is optional)
#ns.gravity              = 0.0  # forcing term is rho * abs(gravity) "down"

#**********************************
#GEOMETRY
#**********************************

geometry.coord_sys   =  0 # 0 for x-y (x-y-z), 1 for r-z
geometry.prob_lo     =  -1. 0. # physical dimensions of the low end of the domain
geometry.prob_hi     =  1. 1. # physical dimensions of the high end of the domain

# >>>>>>>>>>>>>  BC FLAGS <<<<<<<<<<<<<<<<
# 0 = Interior/Periodic  3 = Symmetry
# 1 = Inflow             4 = SlipWall
# 2 = Outflow            5 = NoSlipWall

ns.lo_bc             = 0 5  # boundary conditions on the low end of the domain
ns.hi_bc             = 0 5  # boundary conditions on the high end of the domain

-geometry.period_0    # uncomment if periodic in the x-dir, else comment out
#-geometry.period_1    # uncomment if periodic in the y-dir, else comment out
#-geometry.period_2    # uncomment if periodic in the z-dir, else comment out

#**********************************
#MORE OUTPUT CONTROLS
#**********************************
#
# Select form of FAB output: default is NATIVE
#
#   ASCII  (this is very slow)
#   NATIVE (native binary form on machine -- the default)
#   IEEE32 (useful if you want 32bit files when running in double precision)
#   8BIT   (eight-bit run-length-encoded)
#
fab.format = NATIVE

#**********************************
#MORE PARALLEL CONTROLS
#**********************************
#
# Initializes DistributionMapping strategy from ParmParse.
#
# ParmParse options are:
#
#   DistributionMapping.strategy = ROUNDROBIN
#   DistributionMapping.strategy = KNAPSACK
#   DistributionMapping.strategy = RANDOM
#   DistributionMapping.strategy = SIZEBALANCED
#
# The default strategy is ROUNDROBIN.
#
DistributionMapping.strategy = KNAPSACK
DistributionMapping.strategy = ROUNDROBIN

#**********************************
#GRIDDING & SOLVER CONTROLS (these are optional)
#**********************************

amr.blocking_factor  = 4   # factor by which every patch must be coarsenable
amr.n_error_buf      = 2 2 # number of buffer cells around each tagged cell

# necessary for convergence with this problem
mg.usecg = 0              
mg.nu_f = 100

# USE THIS ONLY FOR TESTING
#amr.max_grid_size = 64     # maximum linear dimension of any one grid