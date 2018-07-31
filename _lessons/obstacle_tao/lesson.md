---
layout: page-fullwidth
title: "The Obstacle Problem"
subheadline: "Numerical Optimization"
teaser: "Leveraging interoperability between PETSc/TAO and MFEM"
permalink: "lessons/obstacle_tao/"
use_math: true
lesson: true
header:
 image_fullwidth: "xsdk_logo_wide.png"
---

## At a Glance
<!-- (Expected # minutes to complete) %% temporarily omit -->

|Questions|Objectives|Key Points|
|1. What is optimization?|Understand the basic principles|Optimization seeks to minimize or maximize a cost function with respect to its inputs|
|2. How to use TAO for derivative-based optimization?|Understand the basic TAO interfaces|TAO is a PETSc subpackage for numerical optimization|
|3. What is the effect of second-order information in derivative-based optimization?|Understand how second-derivatives improve performance of optimization algorithms|Hessian information helps the optimization converge to a local optimum rapidly|

**Note:** To run the application in this lesson
```
cd handson/obstacle
make obstacle
./obstacle -tao_monitor -tao_view
```

## Brief Introduction to Optimization

Numerical optimization methods seek to find the input variables (referred to as "control" or "design" variables) that minimize (or maximize) a quantity of interest (such as cost or performance) subject to constraints (such as bounds on resrouces). A general numerical optimization problems is stated as

$$
\begin{align}
  \underset{x}{\text{min}} \quad &f(x) \\\\
  \text{s. t.} \quad             &c(x) \leq 0 \\\\
                                 &h(x) = 0
\end{align}
$$

In this lesson, we will focus on derivative-based optimization methods -- methods that utilize information about the sensitivity of the objective function $$f(x): \in \mathbb{R}^n \rightarrow \mathbb{R}$$ to its inputs $$x \in \mathbb{R}^n$$. In the convex unconstrained case, the derivative of the objective function with respect to the parameters, $$\nabla f(x)}$$, is zero at the every minimum of $$f(x)$$.  Therefore, we can apply a Newton to solve the system of equations $\nabla f(x) = 0$. Consequently, we solve the system $$\nabla^2 f(x^k) p=-\nabla f(x^k)$$ at every Newton iteration for the step direction $$p$$, where $$\nabla^2 f(x^k)$$ is the Hessian at $$x^k$ and $$\nabla f(x^k) $$ is the gradient of the objective function. When the Hessian is not available directly, it can be approximated from the gradient information by using finite difference or by using [quasi-Newton methods][1]. The Newton method is augmented with a globalization technique (such as a line search or a trust region) to ensure that the method converges.

For more information on the subject, we refer the reader to [_Linear and Nonlinear Optimization_ by Igor Griva, Stephen G. Nash and Ariela Sofer][2] 
and [_Numerical Optimization_ by Jorge Nocedal and Stephen Wright][3]. 

## Problem Formulation

The obstacle problem aims to find the equilibrium of an elastic membrane with a fixed boundary that is stretched over a given obstacle. The mathematical formulation seeks to minimize the Dirichlet energy functional subject to constraints associated with the Dirichlet boundary and the obstacle, 

$$
\begin{align}
  \underset{u}{\text{min}} \quad &J(u) = \int_\Omega |\nabla u|^2 dx \\\\
  \text{s. t.} \quad             &u(x) = 0 \; \text{on} \; d\Omega \\\\
                                 &u(x) \geq \phi(x) \; \text{elsewhere}
\end{align}
$$

where $$u$$ represents the control variables, $$x$$ represents the discrete nodal coordinates in the domain $$\Omega$$, and $$\phi(x)$$ is the obstacle function. In the variational formulation, this is equivalent to solving the Laplace equation $$\nabla^2 u = 0$$ with the boundary conditions corresponding to the obstacle.

In this lesson, we will solve the obstacle problem in the optimization formulation using algorithms in [PETSc/TAO][4], while the calculation of the objective (the Dirichlet energy function), its gradient and Hessian, will be performed using [MFEM][5]. We will be using two gradient-based methods available in TAO, one requiring only the gradient information, and the other both the gradient and the Hessian. We will be examining the improvement in convergence achieved by injecting second-order information into the problem.

Source code for this problem is available in [obstacle.cpp]({{ site.baseurl }}{% link _lessons/obstacle_tao/obstacle.cpp %}). 

## The Obstacle Function

The C++ function representing the obstacle in the problem is written as an MFEM `FunctionCoefficient` object. To accomplish this, we first define a simple function that takes in an MFEM `Vector` defining the (x, y, z) coordinate of a single point in the domain, and returns the corresponding obstacle value at that location. 

In this test case, we have defined a ring shaped obstacle at a value of 1.0, centered at the origin of the domain, with outer radius of 4.0 and a ring thickness of 0.05. The ring must have some finite thickness in order to capture sufficient quadrature points in the Finite-Element space. Outside of the ring itself, the obstacle function returns a value of "negative infinity" (as defined by a PETSc constant `PETSC_NINFINITY`). This will help produce a lower-bound vector for TAO that will effectively treat the degrees of freedom outside the obstacle as unbounded below.

```cpp
// Define a ring-shaped obstacle function centered at the origin with radius 0.4
double RingObstacle(const Vector &x) 
{
  double t = 0.05;                            // thickness
  double r = pow(x(0), 2) + pow(x(1), 2);     // radial location of the point from the origin
  double rad = 0.4;                           // outer radius 
  double ul = pow(rad, 2);                    
  double ll = pow(rad-t, 2);
  if ((ll <= r) && (r <= ul)) {
    // point is inside the ring
    return 1.0;
  } else {
    // point is outside the ring
    return PETSC_NINFINITY;
  }
}
```

This ring shaped obstacle function is then converted into a `FunctionCoefficient` object and projected onto a `GridFunction`. Additionally, we also project a zero `ConstantCoefficient` onto the Dirichlet boundary.

```cpp
ConstantCoefficient zero(0.0);
FunctionCoefficient obs(RingObstacle);

GridFunction lb(fespace); // grid function for the lower bound in the optimization
lb.ProjectCoefficient(obs); // project the obstacle onto the grid
lb.ProjectBdrCoefficient(zero, ess_tdof_list); // project the zero Dirichlet bound onto the grid
```

In this lesson, you can define a new obstacle function of your choice and re-run the problem to see how the obstacle changes the solution and the convergence of the optimization.

## Using TAO

Creating, setting up and using a TAO optimization algorithm is similar to using SNES solvers in PETSc. 

First, the user must implement a `FormFunctionGradient()` function that, given a design vector `X`, computes the cost function value and its gradient vector. This is sufficient for quasi-Newton methods that require only the gradient. However, for Newton methods that require the Hessian, an additional `FormHessian()` function must be provided that provides a Hessian matrix to TAO as a PETSc `Mat` object. For matrix-free Hessian implementations, this `Mat` object can be a `MATSHELL` type that contains the matrix-free `MatMult` operation. Examples of these functions are available in [obstacle.cpp](./obstacle.cpp).

With these functions defined, we can now create the TAO solver, configure it, and trigger the solution.

```c
ierr = TaoCreate(PETSC_COMM_WORLD, &tao);CHKERRQ(ierr);
ierr = TaoSetType(tao, TAOBNLS);CHKERRQ(ierr);
ierr = TaoSetInitialVector(tao, X);CHKERRQ(ierr);
ierr = TaoSetObjectiveAndGradientRoutine(tao, FormFunctionGradient, (void*) &user);CHKERRQ(ierr);
ierr = TaoSetHessianRoutine(tao, user.H, user.H, FormHessian, (void*) &user);CHKERRQ(ierr);
ierr = TaoSetVariableBounds(tao, XL, XU);CHKERRQ(ierr);
ierr = TaoSetMonitor(tao, Monitor, &user, NULL);CHKERRQ(ierr);
ierr = TaoSetFromOptions(tao);CHKERRQ(ierr);
ierr = TaoSolve(tao);CHKERRQ(ierr);
```

Note that these operations must be preceded by `PetscInitialize()` and a `PetscFinalize()` must also be called after the solution is over and all PETSc objects are destroyed.


## Compiling and Running

The proble file can be compiled using the [makefile]({{ site.baseurl }}{% link _lessons/obstacle_tao/makefile %}). Ensure that the MFEM variables point to the correct directories,

```
MFEM_DIR = /projects/ATPESC2018/FASTMath/spark/mfem
MFEM_BUILD_DIR = /projects/ATPESC2018/FASTMath/spark/mfem
```

and both `PETSC_ARCH` and `PETSC_DIR` are set correctly in your shell environment. Additionally, the mesh file location

```
const char *mesh_file = "/projects/ATPESC2018/FASTMath/spark/mfem/data/star.mesh";
```

in `obstacle.cpp` must be set correctly to the correct MFEM data folder. The mesh file path can also be determined with the option flag `--mesh` when running the problem.

Finally, run the compiled executable via command

```
./obstacle -tao_monitor -tao_view -tao_type bqnls
```

## Results

Running the binary with the `-tao_type bqnls` flag uses the bounded quasi-Newton Line-Search algorithm in TAO to solve the problem. In this algorithm, the Hessian of the objective function is approximated using a quasi-Newton method -- specifically, we use the [Broyden-Fletched-Goldfarb-Shanno (BFGS) method][6]. The TAO implementation uses limited-memory quasi-Newton methods, where only a limited number of previous steps are used to construct the approximate Hessian (default: 5 steps). This quasi-Newton method for the obstacle problem converges in 293 nonlinear iterations, and the animation below shows the shape of the solution during the optimization.

[<img src="bqnls.gif" width="800">](bqnls.gif)

Running with the `-tao_type bnls` flag uses the bounded Newton-Line-Search algorithm where the exact Hessian of the problem is directly provided by the underlying implementation instead of being approximated from the gradient. This Hessian is then inverted using a conjugate gradient method, with the inexact Cholesky preconditioner available in PETSc. In contrast with the quasi-Newton method, the Newton algorithm converges in only 3 iterations.

|Iteration 1|Iteration 2|Iteration 3|
|:---:|:---:|:---:|
|[<img src="bnls_init.png" width="400">](bnls_init.png)|[<img src="bnls_mid.png" width="400">](bnls_mid.png)|[<img src="bnls_final.png" width="400">](blns_final.gif)|

## Further Reading

[PETSc Manual](http://www.mcs.anl.gov/petsc/petsc-current/docs/manual.pdf)  
[TAO Manual](http://www.mcs.anl.gov/petsc/petsc-current/docs/tao_manual.pdf)

[1]: https://en.wikipedia.org/wiki/Quasi-Newton_method
[2]: https://books.google.com/books/about/Linear_and_Nonlinear_Optimization.html?id=uOJ-Vg1BnKgC
[3]: https://books.google.com/books?id=VbHYoSyelFcC&dq=Nonlinear+Optimization+Nocedal&source=gbs_navlinks_s
[4]: http://www.mcs.anl.gov/petsc
[5]: http://mfem.org
[6]: https://en.wikipedia.org/wiki/Broyden–Fletcher–Goldfarb–Shanno_algorithm
