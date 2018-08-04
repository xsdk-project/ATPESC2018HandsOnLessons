---
layout: page-fullwidth
title: "Using adjoint for PDE-constrained optimization"
subheadline: "Adjoin time dependent differential equations"
permalink: "lessons/adjoint/"
use_math: true
lesson: true
header:
 image_fullwidth: "theta.png"
layout: page-fullwidth
---

## At a Glance
<!-- (Expected # minutes to complete) %% temporarily omit -->

```
Questions                 |Objectives                     |Key Points
--------------------------|-------------------------------|-------------------------------------
How can gradients be      |Know PETSc/TAO's capability for|Adjoint enables dynamic
computed for simulations? |adjoint and optimization       |constrained optimization.
                          |                               |
How difficult is it to    |Understand ingredients needed  |Jacobian is imperative.
use the adjoint method?   |for adjoint calculation        |
                          |                               |
                          |Understand the concern of      |Performance may depend on
                          |checkpointing                  |checkpointing at large scale.
```

**Note:** To begin this lesson...
```
cd {{site.handson_root}}/adjoint
```


## An Inverse Initial Value Problem

This code demonstrates how to solve an inverse initial value problem for a system of time-dependent PDEs on a 2D rectangular grid.
The goal is to determine an optimal initial condition that can minimizes the difference between the simulated result and the reference solution.
We will use this example to illustrate the performance considerations for realistic large-scale applications. In particular, we will show how to play with checkpointing and how to profile/tune the performance.

### Compile the code
The example [ex5opt_ic.c](https://bitbucket.org/petsc/petsc/src/master/src/ts/examples/tutorials/advection-diffusion-reaction/ex5opt_ic.c) can be compiled with

```
make ex5opt_ic
```
To clean the example, do
```
make clean
```

ATPESC participants do not need to compile code because binaries are available in the ATPESC project folder on Cooley.

### Command line options
You can determine the command line options available for this particular example by doing
```
./ex5opt_ic -help
```
and show the options related to TAO only by doing
```
./ex5opt_ic -help | grep tao
```

### Problem being solved

The underlying PDE models reaction and diffusion of two chemical species that can procude a variety of patterns. It is widely used to describe pattern-formation phenomena in biological, chemical and physical systems. The concentrations of the two spieces are calcuated according to the equation

$$
\frac{d\mathbf{u}}{dt} = D_1 \nabla^2 \mathbf{u} - \mathbf{u} \mathbf{v}^2 + 
\gamma(1 -\mathbf{u}) \\
\frac{d\mathbf{v}}{dt} = D_2 \nabla^2 \mathbf{v} + \mathbf{u} \mathbf{v}^2 - 
(\gamma + \kappa)\mathbf{v}
$$

The spatial pattern for the time interval [0,200] seconds is showin in the following figure.

|Figure 1|
|:---:|
|<img src="pde.gif" width="400">|

Given the pattern (observation) at the final time of the simulation, we want to determine the initial pattern that can minimize the difference between the simulated result and the observation.

$$
\text{minimize}_{X_0} \| X - X^{ref}\|
$$

### Run 1: Monitor solution graphically

```
mpiexec -n 4 ./ex5opt_ic -forwardonly -ts_type rk -ts_max_steps 20 -ts_monitor -ts_monitor_draw_solution
```

* `-forwardonly` perform the forward simulation without doing optimization
* `-ts_type rk` changes the time stepping algorithm to a Runge-Kutta method
* `-ts_monitor_draw_solution` monitors the progress for the solution at each time step
* Add `-draw_pause -2` if you want to pause at the end of simulation to see the plot

### Run 2: Optimal checkpointing schedule
By default, the checkpoints are stored in binary files on disk. Of course, this may not be a good choice for large-scale applications running on high-performance machines where I/O cost is significant. We can make the solver use RAM for checkpointing and specify the maximum allowable checkpoints so that an optimal adjoint checkpointing schedule that minimizes the number of recomputations will be generated.

```
mpiexec -n 4 ./ex5opt_ic -ts_type rk -ts_adapt_type none \
                     -ts_max_steps 10 -ts_monitor -ts_adjoint_monitor \
                     -ts_trajectory_type memory -ts_trajectory_max_cps_ram 3 \
                     -ts_trajectory_monitor -ts_trajectory_view
```
The output corresponds to the schedule depicted by the following diagram:

<img src="chkpt.png" width="800">

#### Questions
{% include qanda
    question='What will happen if we add the option `-ts_trajectory_max_cps_disk 2` to specify there are two available slots for disk checkpoints?'
    answer='Looking at the output, we will find that the new schedule uses both RAM and disk for checkpointing and takes two less recomputations.' %}

### Run 3: Monitor the optimization progress

```
./ex5opt_ic -ts_type rk -ts_adapt_type none -ts_max_steps 4 -tao_monitor -tao_view
  0 TAO,  Function value: 8.36006,  Residual: 6.55337
  1 TAO,  Function value: 1.2247,  Residual: 2.03451
  2 TAO,  Function value: 0.28669,  Residual: 0.988
  3 TAO,  Function value: 0.10604,  Residual: 0.506329
  4 TAO,  Function value: 0.0439724,  Residual: 0.305655
  5 TAO,  Function value: 0.00961548,  Residual: 0.194142
  6 TAO,  Function value: 0.00212597,  Residual: 0.0717966
  7 TAO,  Function value: 0.000314925,  Residual: 0.0277359
  8 TAO,  Function value: 2.25713e-05,  Residual: 0.00721522
  9 TAO,  Function value: 4.93656e-06,  Residual: 0.00297588
 10 TAO,  Function value: 5.25895e-07,  Residual: 0.000915948
 11 TAO,  Function value: 1.85135e-07,  Residual: 0.000583128
 12 TAO,  Function value: 5.59173e-08,  Residual: 0.00048044
 13 TAO,  Function value: 1.07054e-08,  Residual: 0.000133039
 14 TAO,  Function value: 4.02034e-09,  Residual: 6.72053e-05
 15 TAO,  Function value: 1.07375e-09,  Residual: 4.23986e-05
 16 TAO,  Function value: 3.21326e-10,  Residual: 2.61398e-05
 17 TAO,  Function value: 8.69953e-11,  Residual: 1.04063e-05
 18 TAO,  Function value: 3.57649e-11,  Residual: 6.74477e-06
 19 TAO,  Function value: 7.34887e-12,  Residual: 3.07887e-06
 20 TAO,  Function value: 1.54676e-12,  Residual: 1.73001e-06
 21 TAO,  Function value: 5.03403e-13,  Residual: 1.19175e-06
 22 TAO,  Function value: 1.42416e-13,  Residual: 4.86789e-07
 23 TAO,  Function value: 5.08422e-14,  Residual: 4.30692e-07
 24 TAO,  Function value: 9.76239e-15,  Residual: 1.98762e-07
 25 TAO,  Function value: 5.17458e-15,  Residual: 1.53495e-07
 26 TAO,  Function value: 5.75693e-16,  Residual: 2.50412e-08
 27 TAO,  Function value: 2.86621e-16,  Residual: 1.54482e-08
 28 TAO,  Function value: 1.87938e-17,  Residual: 7.31909e-09
Tao Object: 1 MPI processes
  type: blmvm
  Gradient steps: 0
  Mat Object: (tao_blmvm_) 1 MPI processes
    type: lmvmbfgs
    rows=8192, cols=8192
      Scale type: diagonal
      Scale history: 1
      Scale params: alpha=1., beta=0.5, rho=1.
      Convex factors: phi=0., theta=0.125
      Max. storage: 5
      Used storage: 5
      Number of updates: 27
      Number of rejects: 0
      Number of resets: 1
  TaoLineSearch Object: 1 MPI processes
    type: more-thuente
    maximum function evaluations=30
    tolerances: ftol=0.0001, rtol=1e-10, gtol=0.9
    total number of function evaluations=0
    total number of gradient evaluations=0
    total number of function/gradient evaluations=1
    using variable bounds
    Termination reason: 1
  Active Set subset type: subvec
  convergence tolerances: gatol=1e-08,   steptol=0.,   gttol=0.
  Residual in Function/Gradient:=7.31909e-09
  Objective value=1.87938e-17
  total number of iterations=28,                          (max: 2000)
  total number of function/gradient evaluations=29,      (max: 4000)
  Solution converged:    ||g(X)|| <= gatol
```
* `-tao_draw_solution` can visualize the solution at each optimization iteration

#### Questions
{% include qanda
    question='Examine the source code and find the user-provided functions for TAO, TS, and TSAdjoint respectively.'
    answer='Essential functions we have provided are FormFunctionGradient for TAO, TSIFunction and TSIJacobian for TS,  RHSJacobianP for TSAdjoint. Because of the integral in the objective function, extra functions including CostIntegrand, DRDYFunction and DRDPFunction are given to TSAdjoint.' %}

### Run 4: Implicit time integration method
Now we switch to an implicit method ([Crank-Nicolson](https://en.wikipedia.org/wiki/Crank–Nicolson_method)) using fixed stepsize, which is the default setting in the code. At each time step, a nonlinear system is solved by the PETSc nonlinear solver `SNES`.
```
mpiexec -n 12 ./ex5opt_ic -implicit-form -da_grid_x 64 -da_grid_y 64 -ts_max_steps 5 -log_view -tao_monitor
```
* `-snes_monitor` can show the progress of `SNES`
* `-ts_monitor` can show the progress of `TS`
* `-log_view` prints a summary of the logging

A snippet of the summary:
```
...
Phase summary info:
   Count: number of times phase was executed
   Time and Flop: Max - maximum over all processors
                   Ratio - ratio of maximum to minimum over all processors
   Mess: number of messages sent
   Avg. len: average message length (bytes)
   Reduct: number of global reductions
   Global: entire computation
   Stage: stages of a computation. Set stages with PetscLogStagePush() and PetscLogStagePop().
      %T - percent time in this phase         %F - percent flop in this phase
      %M - percent messages in this phase     %L - percent message lengths in this phase
      %R - percent reductions in this phase
   Total Mflop/s: 10e-6 * (sum of flop over all processors)/(max time over all processors)
------------------------------------------------------------------------------------------------------------------------
Event                Count      Time (sec)     Flop                             --- Global ---  --- Stage ---   Total
                   Max Ratio  Max     Ratio   Max  Ratio  Mess   Avg len Reduct  %T %F %M %L %R  %T %F %M %L %R Mflop/s
------------------------------------------------------------------------------------------------------------------------

--- Event Stage 0: Main Stage

VecDot              1130 1.0 3.9795e-01 1.4 1.59e+06 1.0 0.0e+00 0.0e+00 1.1e+03  4  1  0  0  8   4  1  0  0  8    46
VecMDot             1919 1.0 5.9272e-01 2.0 5.40e+06 1.0 0.0e+00 0.0e+00 1.9e+03  5  4  0  0 14   5  4  0  0 14   106
VecNorm             3678 1.0 8.1644e-01 1.3 5.18e+06 1.0 0.0e+00 0.0e+00 3.7e+03  9  4  0  0 27   9  4  0  0 27    74
...
```

#### Questions
{% include qanda
    question='Where is the majority of CPU time spent?'
    answer='Of course answer may vary depending on the settings such as number of procs, problem size, and solver options. Typically most of the time should be spent on [VecMDot](http://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/Vec/VecMDot.html) or [MatMult](http://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/Mat/MatMult.html)' %}

{% include qanda
    question='How can we improve performance?'
    answer='1. Use memory instead of disk for checkpointing(`-ts_trajectory_type memory -ts_trajectory_solution_only 0`); 2. Tune the time stepping solver, nonlinear solver, linear solver, preconditioner and so forth.' %}

### Run 5: Scale up the problem
We use explicit Runge-Kutta methods for time integration, and increase the grid resolution to $512\times 512$.
```
mpiexec -n 12 ./ex5opt_ic -ts_type rk -ts_adapt_type none -ts_max_steps 5 -tao_monitor -da_grid_x 512 -da_grid_y 512
```

#### Questions
{% include qanda
    question='Does the optimization converge? If not, can you fix it?'
    answer='No. The PDE solution blows up. As we decrease the grid spacing, the stepsize should be reduced according to CFL condition. For example, adding -ts_dt 0.1 should work.' %}


### Further information
Because this example uses `DMDA`, Jacobian can be efficiently approxiated using finite difference with coloring. You can use the option `-snes_fd_color` to enable this feature.

## Out-Brief

We have used [PETSc](https://www.mcs.anl.gov/petsc/) to demonstrate the adjoint capability as an enabling technology for dynamic-constrained optimization. In particular, we focused on time-depdent problems including complex dynamical systems with discontinuities and a large scale hyperbolic PDE.

We have shown the basic usage of the adjoint solver as well as functionalities that can facilitate rapid development, diagnosis and performance profiling.

## Further Reading

[PETSc Documentation](http://www.mcs.anl.gov/petsc/documentation/)

<!-- Insert space, horizontal line, and link to HandsOnLesson table -->

&nbsp;

---

[Back to all HandsOnLessons](../lessons.md)
