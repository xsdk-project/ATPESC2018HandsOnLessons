---
layout: page-fullwidth
title: "Sparse, Direct Solvers"
subheadline: "Role and Use of Direct Solvers in Ill-Conditioned Problems"
permalink: "lessons/superlu_mfem/"
use_math: true
lesson: true
answers_google_form: "https://docs.google.com/forms/d/e/1FAIpQLSfxlKXG74hffseYxc52l7p7DALHk-WTiZXQmdT6WGMVBRw7Sg/viewform?usp=sf_link"
header:
 image_fullwidth: "matrices.png"
---

## At A Glance

|Questions|Objectives|Key Points|
|1. Why use a direct solver?|Understand accuracy|Direct solvers are robust<br>for difficult problems|
|2. What effects direct solve performance ?|Understand ordering options|Time & space performance<br>can vary a lot.|

## To begin this lesson

- [Open the Answers Form](https://docs.google.com/forms/d/e/1FAIpQLSfxlKXG74hffseYxc52l7p7DALHk-WTiZXQmdT6WGMVBRw7Sg/viewform?usp=sf_link){:target="_blank"}
- Get into the correct directory
```
cd {{site.handson_root}}/superlu_mfem
```
## The problem being solved

The [convdiff.c](https://github.com/mfem/mfem/blob/atpesc-dev/examples/atpesc/superlu/convdiff.cpp)
application is modeling the steady state convection-diffusion equation in 2D
with a constant velocity.  This equation is used to model the concentration
of something like a _die_ in a _moving_ fluid as it diffuses and flows through 
he fluid.  The equation is as follows:

$$\nabla \cdot (\kappa \nabla u) - \nabla \cdot (\overrightarrow{v}u)+R=0$$

Where _u_ is the concentration that we are tracking, $$\kappa$$, is the diffusion rate,
_v_ is the velocity of the flow and _R_ is a concentration source.
 
In the application we use here, the velocity vector _direction_ is fixed in the _+x_ 
direction. However, the _magnitude_ is set by the user (default of 100), $$\kappa$$,
is fixed at 1.0, and the source is 0.0 everywhere except for a small disc centered at
the middle of the domain where it is 1.0.

|Initial Condition|
|:---:|
|[<img src="mfem-superlu0000.png" width="400">](mfem-superlu0000.png)|
 
Solving this PDE is well known to cause convergence problems for iterative solvers,
for larger _v_. We use MFEM as a vehicle to demonstrate the use of a distributed,
direct solver, [SuperLU_DIST](http://crd-legacy.lbl.gov/~xiaoye/SuperLU/),
to solve very ill-conditioned linear systems.

## Running the Example

### Run 1: default setting with GMRES solver, preconditioned by hypre, velocity = 100

```
$ ./convdiff
Options used:
   --refine 0
   --order 1
   --velocity 100
   --no-visit
   --no-superlu
   --slu-colperm 4
   --slu-rowperm 1
   --slu-parsymbfact 0
   --one-matrix
   --one-rhs
Number of unknowns: 10201
Time required for first solve:  0.0477607 (s)
Final L2 norm of residual: 2.43686e-16
```

|Steady State|
|:---:|
|[<img src="mfem-superlu0005.png" width="400">](mfem-superlu0005.png)|

---

### Run 2: increase velocity to 1000, GMRES does not converge anymore

```
$ ./convdiff --velocity 1000
Options used:
   --refine 0
   --order 1
   --velocity 1000
   --no-visit
   --no-superlu
   --slu-colperm 4
   --slu-rowperm 1
   --slu-parsymbfact 0
   --one-matrix
   --one-rhs
Number of unknowns: 10201
Time required for first solve:  0.564898 (s)
Final L2 norm of residual: 0.00095
```

{% include qanda
   question='How many orders of magnitude different is L2 norm of the residual as compared to the previous run?'
   answer='Between 12 and 13' %}

Below, we plot behavior of the GMRES method for velocity values in the
range [100,1000] at incriments, _dv_, of 25 and also show an animation
of the solution GMRES gives as velocity increases

|Solutions @_dv_=25 in [100,1000]|Contours of Solution @ _vel=1000_|
|:---:||:---:|
|<video src="gmres.mpg" width="400" height="300" controls preload></video>|[<img src="mfem-superlu0003.png" width="400">](mfem-superlu0003.png)|

|Time to Solution|L2 norm of final residual|
|:---:||:---:|
|[<img src="gmres_time.png" width="400">](gmres_time.png)|[<img src="gmres_residual.png" width="400">](gmres_residual.png)|

{% include qanda
   question='What do you think happened?'
   answer='GMRES method works ok for low velocity values.
           As velocity increases, GMRES method eventually crosses a
           threshold where it can no longer provide a useful result' %}

{% include qanda
   question='Why does time to solution show smoother transition than L2 norm?'
   answer='As instability is approached, more GMRES iterations are required to
           reach desired norm. So GMRES is still able to manage the solve and
           achieve a near-zero L2 norm. It just takes more and more iterations.
           Once GMRES is unable to solve the L2 norm explodes.' %}
   
---

### Run 3: Now use SuperLU_DIST, with default options
```
$ ./convdiff --velocity 1000 -slu -cp 0
Options used:
   --refine 0
   --order 1
   --velocity 1000
   --no-visit
   --superlu
   --slu-colperm 0
   --slu-rowperm 1
   --slu-parsymbfact 0
   --one-matrix
   --one-rhs
Number of unknowns: 10201
Time required for first solve:  19.82 (s)
Final L2 norm of residual: 1.62703e-18
```

|Stead State For _vel=1000_|
|:---:|
|[<img src="mfem-superlu0004.png" width="400">](mfem-superlu0004.png)|

### Run 4: Now use SuperLU_DIST, with MMD(A'+A) ordering.
```
./convdiff --velocity 1000 -slu -cp 2
Options used:
   --refine 0
   --order 1
   --velocity 1000
   --no-visit
   --superlu
   --slu-colperm 2
   --slu-rowperm 1
   --slu-parsymbfact 0
   --one-matrix
   --one-rhs
Number of unknowns: 10201
Time required for first solve:  0.100353 (s)
Final L2 norm of residual: 1.53726e-18
```
NOTE: the number of nonzeros in L+U is much smaller than natural ordering.
This affects the memory usage and runtime.

### Run 5: Now use SuperLU_DIST, with Metis(A'+A) ordering.
```
$ ./convdiff -slu --velocity 1000 --slu-colperm 4

Options used:
   --refine 0
   --order 1
   --velocity 1000
   --no-visit
   --superlu
   --slu-colperm 4
Number of unknowns: 10201
       Nonzeros in L       522306
       Nonzeros in U       527748
       nonzeros in L+U     1039853
       nonzeros in LSUB    218211

** Memory Usage **********************************
** NUMfact space (MB): (sum-of-all-processes)
    L\U :            9.24 |  Total :    15.64
** Total highmark (MB):
    Sum-of-all :    15.64 | Avg :    15.64  | Max :    15.64
**************************************************
Time required for solver:  0.786936 (s)
Final L2 norm of residual: 1.55331e-18
```

|Solutions @_dv_=25 in [100,1000]|Steady State Solution @ _vel=1000_|
|:---:||:---:|
|<video src="slu_metis.mpg" width="400" height="300" controls preload></video>|[<img src="mfem-superlu0004.png" width="400">](mfem-superlu0004.png)|

|Time to Solution|
|:---:|
|[<img src="slu_metis_time.png" width="400">](slu_metis_time.png)|

### Run 6: Now use SuperLU_DIST, with Metis(A'+A) ordering, using 16 MPI tasks, on a larger problem.

By adding `--refine 2`, each element in the mesh is subdivided twice yielding a 16x larger problem.
Here, we'll run on 16 tasks and just grep the output form some key values of interest.

```
$ mpiexec -n 16 ./convdiff --refine 2 --velocity 1000 -slu --slu-colperm 4 >& junk.out
$ grep 'Time required for solver:' junk.out 
Time required for solver:  10.3593 (s)
Time required for solver:  16.3567 (s)
Time required for solver:  11.6391 (s)
Time required for solver:  10.669 (s)
Time required for solver:  10.0605 (s)
Time required for solver:  10.1216 (s)
Time required for solver:  20.0721 (s)
Time required for solver:  10.6205 (s)
Time required for solver:  13.8445 (s)
Time required for solver:  11.8943 (s)
Time required for solver:  16.1552 (s)
Time required for solver:  13.0849 (s)
Time required for solver:  14.0008 (s)
Time required for solver:  13.238 (s)
Time required for solver:  12.387 (s)
Time required for solver:  9.81836 (s)
$ grep 'Final L2 norm of residual:' junk.out
Final L2 norm of residual: 3.06951e-18
Final L2 norm of residual: 3.06951e-18
Final L2 norm of residual: 3.06951e-18
Final L2 norm of residual: 3.06951e-18
Final L2 norm of residual: 3.06951e-18
Final L2 norm of residual: 3.06951e-18
Final L2 norm of residual: 3.06951e-18
Final L2 norm of residual: 3.06951e-18
Final L2 norm of residual: 3.06951e-18
Final L2 norm of residual: 3.06951e-18
Final L2 norm of residual: 3.06951e-18
Final L2 norm of residual: 3.06951e-18
Final L2 norm of residual: 3.06951e-18
Final L2 norm of residual: 3.06951e-18
Final L2 norm of residual: 3.06951e-18
Final L2 norm of residual: 3.06951e-18
```

{% include qanda
    question='Can you explain the processor times _relative_ to
              the previous, single processor run?'
    answer='We have increased the mesh size by 16x here. But, we have
            also added 16x processors. Yet, the time for those processors to
            run ranged between 10 and 20 seconds with an average of 12.7 seconds.
            The smaller, single processor run took 0.786936 and taking the ratio
            of these numbers, we get ~16. However, recall that the matrix size
            goes up as the SQUARE of the mesh size and this accounts for this
            additional factor of 16.' %}

---

## Out-Brief

In this lesson, we have used [MFEM](http://mfem.org) as a vehicle to demonstrate
the value of direct solvers from the [SuperLU_DIST](http://crd-legacy.lbl.gov/~xiaoye/SuperLU/)
numerical package.

### Further Reading

To learn more about sparse direct solver, see Gene Golub SIAM Summer School
course materials:
[Lecture Notes](http://www.siam.org/students/g2s3/2013/lecturers/XSLi/Lecture-Notes/sherry.pdf),
[Book Chapter](http://crd-legacy.lbl.gov/~xiaoye/g2s3-summary.pdf), and
[Video](http://www.siam.org/students/g2s3/2013/course.html)
