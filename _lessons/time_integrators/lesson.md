---
layout: page-fullwidth
title: "Time Integration & Non-Linear Solvers"
permalink: "lessons/time_integrators/"
use_math: true
lesson: true
header:
 image_fullwidth: "theta.png"
---
## At a Glance

|Questions|Objectives|Key Points|
|How does the choice of<br>explicit vs. implicit impact step size.|Compare performance of explicit<br>and implicit methods at step<br>sizes near the stability limit.|Time integration considerations<br>play a role in time to solution.|
|What is the impact of an<br>adaptive time integrator?|Compare fixed and adaptive time<br>integrator techniques.|The PETSc package has robust<br>and flexible methods for time integration.|
|How does time integration<br>order impact cost?|Observe impact of order<br>on time to solution/flop<br>and number of steps.|In well-designed packages, changing<br>between methods does not require a lot of effort.|
|Observe quadratic convergence <br> of Newton's method| Observe mesh independent convergence <br> of Newton's method |||

**Note:** To begin this lesson...

cd examples/atpesc/petsc

TODO: REVISE THIS INFO
```

## The problem being solved

The example application here, [advection-ode.cpp][3] uses [MFEM][2] and the ODE solvers from [PETSc][1]
to demonstrate the use of [PETSc][1] in both serial and parallel for more robust and flexible control
over _time integration_ (e.g. discretization in time) of PDEs.

The application has been designed to solve a far more general form of the
[_Advection Equation_](https://en.wikipedia.org/wiki/Heat_equation) in 1, 2 or
3 dimensions as well as to work in a scalable, parallel way.

$$\frac{\partial u}{\partial t} + \vec{v} \cdot \nabla u = 0$$

where $$v$$ is a given fluid velocity and $$u0(x)=u(u,x)$$ is a given initial condition.

Here, all the runs solve a problem on a periodic, hexagonally bounded mesh with an initial
rounded step function of amplitude 1.0 slightly off-center as pictured in Figure 1.

|Figure 1|Figure 2|Figure 3|
|:---:|:---:|:---:|
|[<img src="advection-ode-initial.png" width="400">](advection-ode-initial.png)|[<img src="advection-ode-2.5D.png" width="400">](advection-ode-2.5D.png)|[<img src="advection-ode-animation.gif" width="400">](advection-ode-animation.gif)|

The main loop of [advection-ode.cpp][3]
is shown here...

```c++
   // Explicitly perform time-integration (looping over the time iterations, ti,
   // with a time-step dt), or use the Run method of the ODE solver class.
   if (use_step)
   {
      bool done = false;
      for (int ti = 0; !done; )
      {
         double dt_real = min(dt, t_final - t);
         ode_solver->Step(*U, t, dt_real);
         ti++;

         done = (t >= t_final - 1e-8*dt);

         if (done || ti % vis_steps == 0)
         {
            if (myid == 0)
            {
               cout << "time step: " << ti << ", time: " << t << endl;
            }
            // 11. Extract the parallel grid function corresponding to the finite
            //     element approximation U (the local solution on each processor).
            *u = *U;

         }
      }
   }
   else { ode_solver->Run(*U, t, dt, t_final); }
```

Later in this lesson, we'll show the lines of code that permit the application great
flexibility in how it employs [PETSc][1] to handle time integration.

### Getting Help

You can get help on all the command-line options to this application like so...

```
./advection-ode  --help

Usage: ./advection-ode [options] ...
Options:
   -h, --help
	Print this help message and exit.
   -m <string>, --mesh <string>, current value: ../../../data/periodic-hexagon.mesh
	Mesh file to use.
   -p <int>, --problem <int>, current value: 0
	Problem setup to use. See options in velocity_function().
   -rs <int>, --refine-serial <int>, current value: 2
	Number of times to refine the mesh uniformly in serial.
   -rp <int>, --refine-parallel <int>, current value: 0
	Number of times to refine the mesh uniformly in parallel.
   -o <int>, --order <int>, current value: 3
	Order (degree) of the finite elements.
   -s <int>, --ode-solver <int>, current value: 4
	ODE solver: 1 - Forward Euler,
	            2 - RK2 SSP, 3 - RK3 SSP, 4 - RK4, 6 - RK6.
   -tf <double>, --t-final <double>, current value: 10
	Final time; start time is 0.
   -dt <double>, --time-step <double>, current value: 0.01
	Time step.
   -vis, --visualization, -no-vis, --no-visualization, current option: --visualization
	Enable or disable GLVis visualization.
   -visit, --visit-datafiles, -no-visit, --no-visit-datafiles, current option: --visit-datafiles
	Save data files for VisIt (visit.llnl.gov) visualization.
   -vs <int>, --visualization-steps <int>, current value: 50
	Visualize every n-th timestep.
   -usestep, --usestep, -no-step, --no-step, current option: --usestep
	Use the Step() or Run() method to solve the ODE system.
   -implicit, --implicit, -no-implicit, --no-implicit, current option: --no-implicit
	Use or not an implicit method in PETSc to solve the ODE system.
```

### Run 1: Explicit, Fixed $$\Delta t$$ of 0.001

```
PUT COMMAND AND OUTPUT HERE
```

The first few time steps of this explicit algorithm are plotted below.

|Time Step 0|Time Step 1|Time Step 2|
|:---:|:---:|:---:|
|[<img src="mfem_sundials_explicit0000.png" width="400">](mfem_sundials_explicit0000.png)|[<img src="mfem_sundials_explicit0001.png" width="400">](mfem_sundials_explicit0001.png)|[<img src="mfem_sundials_explicit0002.png" width="400">](mfem_sundials_explicit0002.png)

{% include qanda
    question='What do you think happened"'
    answer='The explicit algorithm is unstable for the specified timestep size.' %}

{% include qanda
    question='How can we make this explicit method work?'
    answer='We can shrink the timestep.' %}

---

### Run 2: Explicit, Smaller $$\Delta t$$ of 0.005

```
PUT COMMAND AND OUTPUT HERE
```

|Time Step 7|Time Step 32|Time Step 100|
|:---:|:---:|:---:|
|[<img src="mfem_sundials_explicit20000.png" width="400">](mfem_sundials_explicit20000.png)|[<img src="mfem_sundials_explicit20001.png" width="400">](mfem_sundials_explicit20001.png)|[<img src="mfem_sundials_explicit20002.png" width="400">](mfem_sundials_explicit20002.png)

---

### Run 3: Explicit, Adaptive $$\Delta t$$ Absolute and Relative Tolerance 1e-6

Now, we will switch to an _adaptive_ time stepping method using the `-adt`
command-line option but keeping all other options the same.

```
PUT COMMANDS AND OUTPUT HERE
```

| Plot of $$\Delta t$$ vs _t_|
|:---:|
|[<img src="mfem_sundials_dtt0000.png" width="400">](mfem_sundials_dtt0000.png)|

{% include qanda
    question='How many steps did this method take and how many flops?'
    answer='By adapting the timestep, we took only 136 steps to the solution
            at t=0.1. There are about 10% more flops required, however, in total.' %}

{% include qanda
    question='A plot of the time step size vs. time is shown above.
              Can you explain its shape?'
    answer='Recall the explicit method we are using here is unstable
            for dt=0.001 but is stable for dt=0.0005. The adaptive algorithm is
            attempting to adapt the timestep. It attemps step sizes outside of
            the regime of stability and may even get away with a few steps in
            that regime before having to shrink the timestep back down to maintain
            stability. Note the majority of timestep sizes used are between 0.0007
            and 0.0008 which is just butting up against the timestep threshold of
            stability for this particular 4th order algorithm.' %}

---

### Run 4: Implicit, Fixed $$\Delta t$$ at 0.001

Now, lets switch to an _implicit_ method and see how that effects behavior of the numerical algorithms.

```
PUT COMMANDS AND OUTPUT HERE
```

Take note of the number of non-linear solution iterations here, 524 and total flops 248347397.

{% include qanda
    question='How does the flop count in this _implicit_ fixed time step method
              compare with the _explicit_ fixed time step method?'
    answer='Well, at a dt=0.001, the explicit method failed due to stability issues.
            But, it suceeded with a dt=0.0005, which required twice as many steps to
            reach t=0.1 where the _implicit_ succeeds with dt=0.001 but still requires
            about 33% more flops due to the implicit solves needed on each step.
            The cost of the implicit method may be reduced by using looser tolerances
            if those are appropriate for the application needs.' %}

{% include qanda
    question='How is all the flexiblity demonstrated here possible?'
    answer='The lines of code below illustrate how the application is
            taking advantage of the SUNDIALS numerical package to affect
            various methods of solution.' %}

---
```c++
204    // Define the ARKODE solver used for time integration. Either implicit or explicit.
205    ODESolver *ode_solver = NULL;
206    ARKODESolver *arkode = NULL;
207    SundialsJacSolver sun_solver; // Used by the implicit ARKODE solver.
208    if (implicit)
209    {
210       arkode = new ARKODESolver(MPI_COMM_WORLD, ARKODESolver::IMPLICIT);
211       arkode->SetLinearSolver(sun_solver);
212    }
213    else
214    {
215       arkode = new ARKODESolver(MPI_COMM_WORLD, ARKODESolver::EXPLICIT);
216       //arkode->SetERKTableNum(FEHLBERG_13_7_8);
217    }
218    arkode->SetStepMode(ARK_ONE_STEP);
219    arkode->SetSStolerances(arkode_reltol, arkode_abstol);
220    arkode->SetOrder(arkode_order);
221    arkode->SetMaxStep(t_final / 2.0);
222    if (!adaptdt)
223    {
224       arkode->SetFixedStep(dt);
225    }
226    ode_solver = arkode;
```

Note lines 210/215 that select either _implicit_ or _explicit_ method.
Lines 220 sets the order of the method and line 224 sets a _fixed_ time
step whereas the default is an _adaptive_ timestep.

---

### Run 5: Implicit, Fixed $$\Delta t$$  at 0.001, 2nd Order

Here, we reduce the order of the time-integration from 4 to 2 and observe the behavior.

```
PUT PROGRAM AND OUTPUT HERE
```

{% include qanda
    question='This second order method succeeds in about 1/2 the number of
              flops and nonlinear iterations. Why?'
    answer='High order time-integration is not always required or the best approach.
            By paying attention to the time-integration order requirements of your
            particular application, you can indeed reduce flop counts required.' %}

---

### Run 6: Implicit, Adaptive $$\Delta t$$, Tolerances 1e-6, 4th Order

In this run, we'll combine **both** the advantages of an _implicit_ algorithm and an _adaptive_ time step.

```
COMMAND TO EXECUTE AND OUTPUT HERE
```

| Plot of $$\Delta t$$ vs _t_|
|:---:|
|[<img src="mfem_sundials_dtt0001.png" width="400">](mfem_sundials_dtt0001.png)|

{% include qanda
    question='How many steps and flops does it take to reach t=0.1?'
    answer='The algorithm reaches t=0.1 in 26 steps but about the same number of flops.' %}

{% include qanda
    question='A plot of dt vs. time is shown above. Why is it able to continue increasing dt?'
    answer='This is using a stable implicit method. As time increases, the solution changes
            more slowly allow the time-integration to continue to increase. Contrast this
            with adaptive time stepping for the explicit case where it would try to increase
            the step size and then constantly have to back up to maintain stability.' %}

---

### Run 7: Implicit, Adaptive $$\Delta t$$, Tolerances 1e-6, 2nd Order

Here, like Run 5, we compare to the preceding run with a 2nd order method.

```
COMMAND AND OUTPUT HERE
```

{% include qanda
    question='Why does the algorithm take more steps to reach t=0.1?'
    answer='The lower order (2 as compared to 4) we are using here means the algorithm is
            unable to maintain the desired tolerances at larger step sizes. In general,
            the step sizes are smaller for a lower order method so more steps are required
            to reach t=0.1' %}

| Plot of $$\Delta t$$ vs _t_|
|:---:|
|[<img src="mfem_sundials_dtt0002.png" width="400">](mfem_sundials_dtt0002.png)|

---

### Run 8: Implicit, Adaptive $$\Delta t$$, Tolerances 1e-6, 5th Order

Here, for a final comparison, we _increase_ to order 5 and observe the impact.

```
COMMAND AND OUTPUT HERE
```

A comparison of the three, preceding _implicit_, adaptive methods at order 4, 2 and 5 is shown below

#### Plot of $$\Delta t$$ vs _t_ for 3 succeeding implicit, adaptive runs

|O=2, N=128, F=218387843|O=4, N=26, F=115569416|O=5, N=15, F=108013134|
|N=_Nn_, F=_Fn_ (normalized)|N=0.2 _Nn_, F=0.5 _Fn_|N=0.12 _Nn_, F=0.5 _Fn_|
|:---:|:---:|:---:|
|[<img src="mfem_sundials_dtt0002.png" width="400">](mfem_sundials_dtt0002.png)|[<img src="mfem_sundials_dtt0001.png" width="400">](mfem_sundials_dtt0001.png)|[<img src="mfem_sundials_dtt0003.png" width="400">](mfem_sundials_dtt0003.png)|

---

### Run 20: Quadratic convergence of Newton's method


```
make elasticity-snes
./elasticity-snes
```
### Run 21: Mesh independent convergence of Newton's method

```
./elasticity-snes -rs 2
./elasticity-snes -rs 4
```

## Out-Brief

We have used MFEM as a demonstration vehicle for illustrating the value in robust,
time integration methods in numerical algorithms. In particular, we have used
the [PETSc][1] solvers to compare and contrast both the effects of _adaptive_ time
stepping as well as the role the order of the time integration plays in time to
solution and number of time steps in the adaptive case.  In addition, we have
demonstrated the ability of implicit methods to run at higher time steps than
explicit and also demonstrated the cost of nonlinear solvers in implicit approaches.

The use of _adaptation_ here was confined to _discretzation_ of time. Other lessons
here demonstrate the advantages _adaptation_ can play in the _discretization_ of
_space_ (e.g. meshing).

Other lessons will demonstrate some the use of other packages _[direct](/lessons/superlu_mfem/)_
and _[iterative](/lessons/krylov_amg/)_ _implicit_ integration approaches.

Finally, it is worth reminding the learner that the application demonstrated here can
be run on 1, 2 and 3 dimensional meshes and in scalable, parallel settings and on meshes
of extremely high spatial resolution if so desired. The learner is encouraged to play
around with various command-line options to affect various scenarios.

### Further Reading

[PETSc Manual](http://www.mcs.anl.gov/petsc/petsc-current/docs/manual.pdf)

[Publications](http://www.mcs.anl.gov/petsc/publications/index.html)

[1]: http://www.mcs.anl.gov/petsc
[2]: http://mfem.org
[3]: https://github.com/mfem/mfem/blob/atpesc-dev/examples/atpesc/petsc/advection-ode.cpp
