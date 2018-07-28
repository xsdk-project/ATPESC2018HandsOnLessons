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
|What is the impact of an<br>adaptive time integrator?|Compare fixed and adaptive time<br>integrator techniques.|The SUNDIALS package has robust<br>and flexible methods for time integration.|
|How does time integration<br>order impact cost?|Observe impact of order<br>on time to solution/flop<br>and number of steps.|In well-designed packages, changing<br>between methods does not require a lot of effort.|

**Note:** To begin this lesson...
```
cd handson/mfem/examples/atpesc/sundials
TODO: REVISE THIS INFO
```

## The problem being solved

The example application here,
[transient-heat.cpp](https://github.com/mfem/mfem/blob/atpesc-dev/examples/atpesc/sundials/transient-heat.cpp),
uses [MFEM](http://mfem.org) and the ARKode package from SUNDIALS as a vehicle
to demonstrate the use of the
[SUNDIALS](https://computation.llnl.gov/projects/sundials) suite
in both serial and parallel for more robust and flexible control over _time integration_
(e.g. discretization in time) of PDEs.

The application has been designed to solve a far more general form of the
[_Heat Equation_](https://en.wikipedia.org/wiki/Heat_equation) in 1, 2 or
3 dimensions as well as to work in a scalable, parallel way.

$$\frac{\partial u}{\partial t} - \nabla \cdot (\kappa + u \alpha) \nabla u = 0$$


where the material thermal diffusivity is given by $$(\kappa + \alpha{u})$$
which includes the same constant term $$\kappa$$
as in [Lesson 1](../hand_coded_heat/lesson.md) plus a term $$\alpha{u}$$
which varies with
temperature, _u_, introducing the option of solving systems involving non-linearity.

Compare this equation with that of the [hand-coded heat equation](../hand_coded_heat/lesson.md)

$$\frac{\partial u}{\partial t} - \nabla \cdot \alpha \nabla u = 0$$

which we simplifed to...

$$\frac{\partial u}{\partial t} = \alpha \frac{\partial^2 u}{\partial x^2}$$

and we see
[transient-heat.cpp](https://github.com/mfem/mfem/blob/atpesc-dev/examples/atpesc/sundials/transient-heat.cpp)
a _much more generalized_ form of the heat equation than [heat.c](../hand_coded_heat/heat.c.numbered.txt)

* It supports 1, 2 and 3 dimensions
* It supports inhomogeneous materal _thermal diffusivity_
* It supports thermal diffusivity that varies with temperature

Here, all the runs solve a problem with an initial condition is a _pyramid_ with
value of _1_ at the apex in the _middle_ of the computational domain and zero on
the boundaries as pictured in Figure 1.

|Figure 1|Figure 2|
|:---:|:---:|
|[<img src="mfem_sundials_explicit0000.png" width="400">](mfem_sundials_explicit0000.png)|[<img src="pyramid_animated.gif" width="400">](pyramid_animated.gif)|

The main loop of
[transient-heat.cpp](https://github.com/mfem/mfem/blob/atpesc-dev/examples/atpesc/sundials/transient-heat.cpp)
is shown here...

```c++
304    // Perform time-integration
309    ode_solver->Init(oper);
310    double t = 0.0;
311    bool last_step = false;
312    for (int ti = 1; !last_step; ti++)
313    {
319       ode_solver->Step(u, t, dt);
320
327       u_gf.SetFromTrueDofs(u);
328
336       oper.SetParameters(u);
337       last_step = (t >= t_final - 1e-8*dt);
338    }
```

Later in this lesson, we'll show the lines of code that permit the
application great flexibility in how it employs
[SUNDIALS](https://computation.llnl.gov/projects/sundials) to handle
time integration.

### Getting Help
```
PUT NEW COMMAND AND OUTPUT HERE
```

**Note:** This application may be used to solve the same equation used in
[Lesson 1](../hand_coded_heat/lesson.md) by using command line options
`-d 1 -alpha 0`. The role of [Lesson 1's](../hand_coded_heat/lesson.md)
$$\alpha$$, is played by $$\kappa$$ here.
For all of the runs here, the application's default behavior is to set
$$\alpha$$ to 0.2 and $$\kappa$$ to 0.5.

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

## Out-Brief

We have used MFEM as a demonstration vehicle for illustrating the value in robust,
time integration methods in numerical algorithms. In particular, we have used
the [SUNDIALS](https://computation.llnl.gov/projects/sundials) suite of solvers to
compare and contrast both the effects of _adaptive_ time stepping as well as the
role the order of the time integration plays in time to solution and number of time
steps in the adaptive case.  In addition, we have demonstrated the ability of implicit
methods to run at higher time steps than explicit and also demonstrated the cost of
nonlinear solvers in implicit approaches.

The use of _adaptation_ here was confined to _discretzation_ of time. Other lessons
here demonstrate the advantages _adaptation_ can play in the _discretization_ of
_space_.

Other lessons will demonstrate some of the options for _nonlinear_ and
_linear_ solvers needed for implicit integration approaches.

Finally, it is worth reminding the learner that the application demonstrated here can
be run on 1, 2 and 3 dimensional meshes and in scalable, parallel settings on on meshes
of extremely high spatial resolution if so desired. The learner is encouraged to play
around with various command-line options to affect various scenarios.

### Further Reading

[Users guides for CVODE, ARKode, and IDA](https://computation.llnl.gov/projects/sundials/sundials-software)

[Publications](https://computation.llnl.gov/projects/sundials/publications)