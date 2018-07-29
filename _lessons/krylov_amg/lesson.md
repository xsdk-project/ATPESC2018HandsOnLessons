---
layout: page-fullwidth
title: "Krylov Solvers and Algebraic Multigrid"
subheadline: "Demonstrate utility of multigrid"
permalink: "lessons/krylov_amg/"
use_math: true
lesson: true
header:
 image_fullwidth: "2012.jpg"
---

## At a Glance

|Why multigrid over a Krylov<br>solver for large problems?|Understand multigrid concept.|Faster convergence,<br>better scalability.|
|Why use more aggresive<br>coarsening for AMG?|Understand need for low complexities.|Lower memory use, faster times,<br>but more iterations.|
|Why a structured solver<br>for a structured problem?|Understand importance of<br>suitable data structures|Higher efficiency,<br>faster solve times.|


## The Problem Being Solved

The linear system to be solved is generated using central finite differences applied to the Poisson equation

$$-{\Delta u} = f$$

on a cuboid of size $$n_x \times n_y \times n_z$$ with Dirichlet boundary conditions

$$u = 0$$


## The Example Source Code

For the first part of the hands-on lessons we will use the executable ij. Various solver, problem and parameter options can be invoked by adding them to the command line.
A complete set of options will be printed by typing
```
ij -help
```
Here is an excerpt of the output of this command with all the options relevant for the hands-on lessons.

```
Usage: ij [<options>]

Choice of Problem:
  -laplacian [<options>] : build 7pt 3D laplacian problem (default)
  -difconv [<opts>]      : build convection-diffusion problem
    -n <nx> <ny> <nz>    : problem size per process
    -P <Px> <Py> <Pz>    : process topology
    -c <cx> <cy> <cz>    : diffusion coefficients
    -a <ax> <ay> <az>    : convection coefficients

Choice of solver:
   -amg                  : AMG only
   -amgpcg               : AMG-PCG
   -pcg                  : diagonally scaled PCG
   -amggmres             : AMG-GMRES with restart k (default k=10)
   -gmres                : diagonally scaled GMRES(k) (default k=10)
   -amgbicgstab          : AMG-BiCGSTAB
   -bicgstab             : diagonally scaled BiCGSTAB
   -k  <val>             : dimension Krylov space for GMRES

.....

  -tol  <val>            : set solver convergence tolerance = val
  -max_iter  <val>       : set max iterations 
  -agg_nl  <val>         : set number of aggressive coarsening levels (default:0)
  -iout <val>            : set output flag
       0=no output    1=matrix stats
       2=cycle stats  3=matrix & cycle stats

  -print                 : print out the system
```

## Running the Example

### First Set of Runs (Krylov Solvers)

Run the first example for a small problem of size 8000 using restarted GMRES with a Krylov space of size 10.
```
ij -gmres -n 20 20 20 -k 10
```

#### Expected Behavior/Output

You should get something that looks like this
```
Running with these driver parameters:
  solver ID    = 4

    (nx, ny, nz) = (20, 20, 20)
    (Px, Py, Pz) = (1, 1, 1)
    (cx, cy, cz) = (1.000000, 1.000000, 1.000000)

    Problem size = (20 x 20 x 20)

=============================================
Generate Matrix:
=============================================
Spatial Operator:
  wall clock time = 0.000000 seconds
  wall MFLOPS     = 0.000000
  cpu clock time  = 0.000000 seconds
  cpu MFLOPS      = 0.000000

  RHS vector has unit components
  Initial guess is 0
=============================================
IJ Vector Setup:
=============================================
RHS and Initial Guess:
  wall clock time = 0.000000 seconds
  wall MFLOPS     = 0.000000
  cpu clock time  = 0.000000 seconds
  cpu MFLOPS      = 0.000000

Solver: DS-GMRES
HYPRE_GMRESGetPrecond got good precond
=============================================
Setup phase times:
=============================================
GMRES Setup:
  wall clock time = 0.000000 seconds
  wall MFLOPS     = 0.000000
  cpu clock time  = 0.000000 seconds
  cpu MFLOPS      = 0.000000

=============================================
Solve phase times:
=============================================
GMRES Solve:
  wall clock time = 0.050000 seconds
  wall MFLOPS     = 0.000000
  cpu clock time  = 0.040000 seconds
  cpu MFLOPS      = 0.000000


GMRES Iterations = 186
Final GMRES Relative Residual Norm = 9.593291e-09
Total time = 0.050000
```

Note the total time and the number of iterations.
Now increase the Krylov subspace by changing input to -k to 20, then 30, 40, and finally 50.

{% include qanda question='What do you observe about the number of iterations and times?' answer='Number of iterations and times improve' %}

{% include qanda question='How many restarts were required for the last run using -k 50?'  answer='None, since the number of iterations is 49. Here full GMRES was used.'%}

Now increase the problem size to -n 30 30 30 and -n 40 40 40 combined with -k 50.

{% include qanda question='What do you observe about the number of iterations and times?' answer='Number of iterations and times increase.' %}

Now solve the last problem with -n 40 40 40 using -pcg and -bicgstab.

{% include qanda question='What do you observe about the number of iterations and times for all three methods? Which method is the fastest and which one has the lowest number of iterations?' answer='Conjugate gradient has the lowest time, but BiCGSTAB has the lowest number of iterations.' %}

{% include qanda question='Why is BiCGSTAB slower than PCG?' answer='It requires two matrix vector operations and additional vector operations per iteration, and thus each iteration takes longer than an iteration of PCG.' %}


#### Examining Results

---

## Out-Brief

Here, re-emphasize the lesson objectives and key points.

Its fine to go into greater detail about questions or objectives this lesson
did not fully cover.

### Further Reading

Include links to other online sources you might want to include.
