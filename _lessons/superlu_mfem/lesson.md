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
is fixed at 1.0, and the source is 0.0 everywhere except for a small square centered at
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
$ ./convdiff | tail -n 3
Time required for first solve:  0.0408995 (s)
Final L2 norm of residual: 2.43686e-16
```

|Steady State|
|:---:|
|[<img src="mfem-superlu0005.png" width="400">](mfem-superlu0005.png)|

---

### Run 2: increase velocity to 1000, GMRES does not converge anymore

```
$ ./convdiff --velocity 1000 | tail -n 3
Time required for first solve:  0.47337 (s)
Final L2 norm of residual: 0.00095
```

{% include qanda
   question='How many orders of magnitude different is L2 norm of the residual as compared to the previous run?'
   answer='Between 12 and 13' %}

Below, we plot behavior of the GMRES method for velocity values in the
range [100,1000] at increments, _dv_, of 25 and also show an animation
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

### Run 3: Now use SuperLU_DIST, with "natural ordering"
```
$  ./convdiff --velocity 1000 -slu -cp 0  
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
	Nonzeros in L       1040781
	Nonzeros in U       1045632
	nonzeros in L+U     2076212
	nonzeros in LSUB    1040215

** Memory Usage **********************************
** NUMfact space (MB): (sum-of-all-processes)
    L\U :           41.12 |  Total :    50.74
** Total highmark (MB):
    Sum-of-all :    50.74 | Avg :    50.74  | Max :    50.74
**************************************************
Time required for first solve:  19.0018 (s)
Final L2 norm of residual: 1.62703e-18

**************************************************
**** Time (seconds) ****
	EQUIL time             0.00
	ROWPERM time           0.01
	SYMBFACT time          0.04
	DISTRIBUTE time        0.11
	FACTOR time           18.52
	Factor flops	1.958603e+08	Mflops 	   10.58
	SOLVE time             0.10
	Solve flops	5.167045e+06	Mflops 	   52.21
	REFINEMENT time        0.20	Steps       2

**************************************************
```

|Stead State For _vel=1000_|
|:---:|
|[<img src="mfem-superlu0004.png" width="400">](mfem-superlu0004.png)|

### Run 4: Now use SuperLU_DIST, with MMD(A'+A) ordering.
```
$ ./convdiff --velocity 1000 -slu -cp 2
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
	Nonzeros in L       594238
	Nonzeros in U       580425
	nonzeros in L+U     1164462
	nonzeros in LSUB    203857

** Memory Usage **********************************
** NUMfact space (MB): (sum-of-all-processes)
    L\U :           10.07 |  Total :    15.52
** Total highmark (MB):
    Sum-of-all :    15.52 | Avg :    15.52  | Max :    15.52
**************************************************
Time required for first solve:  0.111105 (s)
Final L2 norm of residual: 1.53726e-18

**************************************************
**** Time (seconds) ****
	EQUIL time             0.00
	ROWPERM time           0.01
	COLPERM time           0.04
	SYMBFACT time          0.01
	DISTRIBUTE time        0.02
	FACTOR time            0.05
	Factor flops	1.063303e+08	Mflops 	 2045.75
	SOLVE time             0.00
	Solve flops	2.367059e+06	Mflops 	  779.35
	REFINEMENT time        0.01	Steps       2

**************************************************
```
NOTE: the number of nonzeros in L+U is much smaller than natural ordering.
This affects the memory usage and runtime.

### Run 5: Now use SuperLU_DIST, with Metis(A'+A) ordering.
```
$ ./convdiff --velocity 1000 -slu -cp 4
Options used:
   --refine 0
   --order 1
   --velocity 1000
   --no-visit
   --superlu
   --slu-colperm 4
   --slu-rowperm 1
   --slu-parsymbfact 0
   --one-matrix
   --one-rhs
Number of unknowns: 10201
	Nonzeros in L       522306
	Nonzeros in U       527748
	nonzeros in L+U     1039853
	nonzeros in LSUB    218211

** Memory Usage **********************************
** NUMfact space (MB): (sum-of-all-processes)
    L\U :            9.24 |  Total :    14.96
** Total highmark (MB):
    Sum-of-all :    14.96 | Avg :    14.96  | Max :    14.96
**************************************************
Time required for first solve:  0.152424 (s)
Final L2 norm of residual: 1.51089e-18

**************************************************
**** Time (seconds) ****
	EQUIL time             0.00
	ROWPERM time           0.01
	COLPERM time           0.05
	SYMBFACT time          0.01
	DISTRIBUTE time        0.02
	FACTOR time            0.05
	Factor flops	7.827314e+07	Mflops 	 1717.18
	SOLVE time             0.00
	Solve flops	2.120276e+06	Mflops 	  606.75
	REFINEMENT time        0.01	Steps       2

**************************************************
```

|Solutions @_dv_=25 in [100,1000]|Steady State Solution @ _vel=1000_|
|:---:||:---:|
|<video src="slu_metis.mpg" width="400" height="300" controls preload></video>|[<img src="mfem-superlu0004.png" width="400">](mfem-superlu0004.png)|

|Time to Solution|
|:---:|
|[<img src="slu_metis_time.png" width="400">](slu_metis_time.png)|

### Run 5.5: Now use SuperLU_DIST, with Metis(A'+A) ordering, using 1 MPI tasks, on a larger problem.
By adding `--refine 2`, each element in the mesh is subdivided twice yielding a 16x larger problem.
But, we'll run it on only one processor.

```
$ mpiexec -n 1 ./convdiff --refine 2 --velocity 1000 -slu -cp 4
Options used:
   --refine 2
   --order 1
   --velocity 1000
   --no-visit
   --superlu
   --slu-colperm 4
   --slu-rowperm 1
   --slu-parsymbfact 0
   --one-matrix
   --one-rhs
Number of unknowns: 160801
	Nonzeros in L       8746079
	Nonzeros in U       8746079
	nonzeros in L+U     17331357
	nonzeros in LSUB    3659374

** Memory Usage **********************************
** NUMfact space (MB): (sum-of-all-processes)
    L\U :          152.37 |  Total :   171.34
** Total highmark (MB):
    Sum-of-all :   172.43 | Avg :   172.43  | Max :   172.43
**************************************************
Time required for first solve:  3.28833 (s)
Final L2 norm of residual: 3.07474e-18

**************************************************
**** Time (seconds) ****
	EQUIL time             0.01
	ROWPERM time           0.07
	COLPERM time           1.11
	SYMBFACT time          0.08
	DISTRIBUTE time        0.34
	FACTOR time            1.26
	Factor flops	2.596701e+09	Mflops 	 2056.43
	SOLVE time             0.15
	Solve flops	3.523111e+07	Mflops 	  236.61
	REFINEMENT time        0.25	Steps       2

**************************************************
```

### Run 6: Now use SuperLU_DIST, with Metis(A'+A) ordering, using 16 MPI tasks, on a larger problem.

Here, we'll re-run the above except on 16 tasks and just grep the output form some key values of interest.

```
$ ${MPIEXEC_OMPI} -n 16 ./convdiff --refine 2 --velocity 1000 -slu --slu-colperm 4 >& junk.out
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

### Run 7: Now use SuperLU_DIST, with Metis(A'+A) ordering, using 16 MPI tasks, on a larger problem.

Here, we re-solve the same system a second time except telling SuperLU to re-use the right hand side.
Notice the improvement in solve time when re-using the right hand side.

```
$ mpiexec -n 16 ./convdiff --refine 2 --velocity 1000 -slu -cp 4 -2rhs
Options used:
   --refine 2
   --order 1
   --velocity 1000
   --no-visit
   --superlu
   --slu-colperm 4
   --slu-rowperm 1
   --slu-parsymbfact 0
   --one-matrix
   --two-rhs
Number of unknowns: 160801
	Nonzeros in L       8751360
	Nonzeros in U       8751360
	nonzeros in L+U     17341919
	nonzeros in LSUB    3696221

** Memory Usage **********************************
** NUMfact space (MB): (sum-of-all-processes)
    L\U :          153.37 |  Total :   295.20
** Total highmark (MB):
    Sum-of-all :   703.49 | Avg :    43.97  | Max :    43.97
**************************************************
Time required for first solve:  29.1715 (s)
Final L2 norm of residual: 3.18307e-40

**************************************************
**** Time (seconds) ****
	EQUIL time             0.33
	ROWPERM time           1.49
	COLPERM time           1.64
	SYMBFACT time          0.09
	DISTRIBUTE time        1.23
	FACTOR time           33.39
	Factor flops	2.623572e+09	Mflops 	   78.58
	SOLVE time             0.99
	Solve flops	3.524035e+07	Mflops 	   35.77
	REFINEMENT time        2.16	Steps       2

**************************************************
Time required for second solve (new rhs):  2.8843 (s)
Final L2 norm of residual: 2.98269e-40

	SOLVE time             0.74
	Solve flops	3.524035e+07	Mflops 	   47.75
	REFINEMENT time        1.78	Steps       2

**************************************************
```

### Run 8

```
i$ mpiexec -n 16 ./convdiff --refine 2 --velocity 1000 -slu -cp 4 -2mat
Options used:
   --refine 2
   --order 1
   --velocity 1000
   --no-visit
   --superlu
   --slu-colperm 4
   --slu-rowperm 1
   --slu-parsymbfact 0
   --two-matrix
   --one-rhs
Number of unknowns: 160801
	Nonzeros in L       8751360
	Nonzeros in U       8751360
	nonzeros in L+U     17341919
	nonzeros in LSUB    3696221

** Memory Usage **********************************
** NUMfact space (MB): (sum-of-all-processes)
    L\U :          153.37 |  Total :   295.20
** Total highmark (MB):
    Sum-of-all :   703.49 | Avg :    43.97  | Max :    43.97
**************************************************
Time required for first solve:  35.4073 (s)
Final L2 norm of residual: 2.543e-40

**************************************************
**** Time (seconds) ****
	EQUIL time             0.93
	ROWPERM time           1.98
	COLPERM time           1.86
	SYMBFACT time          0.09
	DISTRIBUTE time        0.33
	FACTOR time           34.80
	Factor flops	2.623572e+09	Mflops 	   75.39
	SOLVE time             0.97
	Solve flops	3.524035e+07	Mflops 	   36.51
	REFINEMENT time        2.12	Steps       2

**************************************************
	Nonzeros in L       8751360
	Nonzeros in U       8751360
	nonzeros in L+U     17341919
	nonzeros in LSUB    3696221

** Memory Usage **********************************
** NUMfact space (MB): (sum-of-all-processes)
    L\U :          153.37 |  Total :   295.20
** Total highmark (MB):
    Sum-of-all :   703.49 | Avg :    43.97  | Max :    43.97
**************************************************
Time required for second matrix (same sparsity):  32.8356 (s)
Final L2 norm of residual: 2.37648e-40
**************************************************
**** Time (seconds) ****
	EQUIL time             0.70
	ROWPERM time           0.31
	COLPERM time           1.98
	SYMBFACT time          0.09
	DISTRIBUTE time        0.91
	FACTOR time           32.69
	Factor flops	2.623572e+09	Mflops 	   80.27
	SOLVE time             0.96
	Solve flops	3.524035e+07	Mflops 	   36.68
	REFINEMENT time        2.16	Steps       2

**************************************************
```

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
