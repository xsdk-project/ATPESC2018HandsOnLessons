---
layout: page-fullwidth
title: "Unstructured Meshing & Discretization with MFEM"
subheadline: "Finite Elements and Convergence"
permalink: "lessons/mfem_convergence/"
use_math: true
lesson: true
header:
 image_fullwidth: "WSC_top4.jpg"
---
## At A Glance

|Questions|Objectives|Key Points|
|What is a finite element method?|Understand basic finite element machinery.|Basis functions determine<br>the quality of the solution.|
|What is a high order method?|Understand how polynomial<br>order affects simulations.|High order methods add more<br>unknowns on the same mesh<br>for more precise solutions.|
|What is _convergence_?|Understand how convergence and<br>convergence rate are calculated.|High order methods converge<br>faster for smooth solutions.|

### To begin this lesson...
- [Open the Answers Form](https://docs.google.com/forms/d/e/1FAIpQLScs9reOCfuD1CfbQ-m458MDyvwiTCRXEcp1XCQukaf5tP_uSQ/viewform?usp=sf_link)
- Add the
```
/projects/ATPESC2018/FASTMath/spack/bin/spack
```
directory to your `PATH`.

- Copy the MFEM install directory locally
```
cp -a `spack location -i mfem` mfem
```

- Go into the ATPESC examples directory
```
cd mfem/examples/atpesc
```

- The MFEM [convergence](https://github.com/mfem/mfem/blob/atpesc-dev/examples/atpesc/mfem/convergence.cpp) example described below is in the `mfem/` directory.

## A Widely Applicable Equation

In this lesson, we demonstrate the discretization of a simple Poisson problem using
the [MFEM library](http://mfem.org) and examine the finite element approximation error
under uniform refinement. An example of this equation is steady-state [heat](../hand_coded_heat/)
[conduction](../time_integrators/).

|[<img src="ex8.png">](ex8.png)| [<img src="diffusion.png">](diffusion.png)|

### Governing Equation

The [_Poisson Equation_](https://en.wikipedia.org/wiki/Poisson's_equation) is a partial
differential equation (PDE) that can be used to model steady-state heat conduction,
electric potentials and gravitational fields. In mathematical terms ...

$$-\nabla^2u = f$$

where _u_ is the potential field and _f_ is the source function. This PDE is a generalization
of the [_Laplace Equation_](https://en.wikipedia.org/wiki/Laplace%27s_equation).

### Finite Element Basics

To solve the above continuous equation using computers we need to
[discretize](https://en.wikipedia.org/wiki/Discretization) it by introducing a finite
(discrete) number of unknowns to compute for.
In the [_Finite Element Method_](https://en.wikipedia.org/wiki/Finite_element_method) (FEM), this is
done using the concept of _basis functions_.

Instead of calculating the exact analytic solution _u_, consider approximating it by

$$u \approx \sum_{j=1}^n c_j \phi_j$$

where $$c_j$$ are scalar unknown coefficients and $$\phi_j$$ are known _basis functions_.
They are typically piecewise-polynomial functions which are only non-zero on small portions of the
computational mesh.

|[<img src="phi.png" width="400">](phi.png)|

With finite elements, the mesh can be totally unstructured, curved and
non-conforming.

|[<img src="mesh.png" width="300">](mesh.png)|

To solve for the unknown coefficients, we multiply Poisson's equation by another (test)
basis function $$\phi_i$$ and integrate by parts to obtain

$$\sum_{j=1}^n\int_\Omega c_j \nabla \phi_j \cdot \nabla \phi_i dV = \int_\Omega f \phi_i$$

for every basis function $$\phi_i$$. (Here we are assuming homogeneous Dirichlet boundary
conditions corresponding, for example, to zero temperature on the whole boundary.)

Since the basis functions are known, we can rewrite (3) as

$$\mathbf{Ax} = \mathbf{b}$$

where

$$A_{ij} = \int_\Omega \nabla \phi_i \cdot \nabla \phi_j dV$$

$$b_i = \int_\Omega f \phi_i dV$$

$$x_j = c_j$$


This is a $$n \times n$$ linear system that can be solved [directly](../superlu_mfem/) or
[iteratively](../krylov_amg/) for the unknown coefficients. Note that we are free
to choose the basis functions $$\phi_i$$ as we see fit.

---

## Convergence Study Source Code

To define the system we need to solve, we need three things. First, we need to define our
basis functions which live on the computational mesh.

```c++
   // order is the FEM basis functions polynomial order
   FiniteElementCollection *fec = new H1_FECollection(order, dim);

   // pmesh is the parallel computational mesh
   ParFiniteElementSpace *fespace = new ParFiniteElementSpace(pmesh, fec);
```

This defines a collection of H1 functions (meaning they have well-defined gradient) of
a given polynomial order on a parallel computational mesh pmesh. Next, we need to define
the integrals in Equation (5)

```c++
   ParBilinearForm *a = new ParBilinearForm(fespace);
   ConstantCoefficient one(1.0);
   a->AddDomainIntegrator(new DiffusionIntegrator(one));
   a->Assemble();
```

and Equation (6)

```c++
   // f_exact is a C function defining the source
   FunctionCoefficient f(f_exact);
   ParLinearForm *b = new ParLinearForm(fespace);
   b->AddDomainIntegrator(new DomainLFIntegrator(f));
   b->Assemble();
```

This defines the matrix A and the vector b. We then solve the linear
system for our solution vector x using [AMG-preconditioned](../krylov_amg/) PCG iteration.

```c++
   // FEM -> Linear System
   HypreParMatrix A;
   Vector B, X;
   a->FormLinearSystem(ess_tdof_list, x, *b, A, X, B);

   // AMG preconditioner
   HypreBoomerAMG *amg = new HypreBoomerAMG(A);
   amg->SetPrintLevel(0);

   // PCG Krylov solver
   HyprePCG *pcg = new HyprePCG(A);
   pcg->SetTol(1e-12);
   pcg->SetMaxIter(200);
   pcg->SetPrintLevel(0);
   pcg->SetPreconditioner(*amg);

   // Solve the system A X = B
   pcg->Mult(B, X);

   // Linear System -> FEM
   a->RecoverFEMSolution(X, *b, x);
```

In this lesson we know what the exact solution is, so we can measure the amount of
error in our approximate solution in two ways:

$$\left \| u_{\mbox{exact}} - u_{\mbox{h}} \right \|_{L_2}^2 = \int_\Omega \left| u_{\mbox{exact}} - u_{\mbox{h}} \right |^2$$

$$\left \| u_{\mbox{exact}} - u_{\mbox{h}} \right \|_{H^1}^2 = \left \| u_{\mbox{exact}} - u_{\mbox{h}} \right \|_{L_2}^2 + \left \| \nabla u_{\mbox{exact}} - \nabla u_{\mbox{h}} \right \|_{L_2}^2$$

We expect the error to behave like

$$\left \| u_{\mbox{exact}} - u_{\mbox{h}} \right \|_{L_2}^2 \leq Ch^{r}$$

where $$h$$ is the mesh size, $$C$$ is a mesh-independent constant and $$r$$
is the [_convergence rate_](https://en.wikipedia.org/wiki/Rate_of_convergence).

Given approximations at two different mesh resolutions, we can  estimate the convergence rate as
follows ($$C$$ doesn't change when we refine the mesh and compare runs):

$$r \approx \frac{\log\ \frac{ \left \| u_{\mbox{exact}} - u_{h_{\mbox{new}}} \right \|_{L_2}}{\left \| u_{\mbox{exact}} - u_{h_{\mbox{old}}} \right \|_{L_2}}}{ \log \frac{h_{\mbox{new}}}{h_{\mbox{old}}}}$$

In code, this is implemented in a refinement loop as follows:

```c++
   double l2_err = x.ComputeL2Error(u);
   double h1_err = x.ComputeH1Error(&u, &u_grad, &one, 1.0, 1);
   pmesh->GetCharacteristics(h_min, h_max, kappa_min, kappa_max);

   l2_rate = log(l2_err/l2_err_prev) / log(h_min/h_prev);
   h1_rate = log(h1_err/h1_err_prev) / log(h_min/h_prev);
```

---

## Running the Convergence Study

The convergence study in `mfem/examples/atpesc/mfem` has the following options.

```
./convergence --help

Usage: ./convergence [options] ...
Options:
   -h, --help
	Print this help message and exit.
   -m <string>, --mesh <string>, current value: ../../../data/star.mesh
	Mesh file to use.
   -o <int>, --order <int>, current value: 1
	Finite element order (polynomial degree).
   -sc, --static-condensation, -no-sc, --no-static-condensation, current option: --no-static-condensation
	Enable static condensation.
   -r <int>, --refinements <int>, current value: 4
	Number of total uniform refinements
   -sr <int>, --serial-refinements <int>, current value: 2
	Maximum number of serial uniform refinements
   -f <double>, --frequency <double>, current value: 1
	Set the frequency for the exact solution.
```

### Run 1 (Low order)

In this run, we will examine the error after seven uniform refinements in both the L2 and H1 norms using
first order (linear) basis functions. We use the `star.mesh` 2D mesh file.

```
./convergence -r 7
Options used:
   --mesh ../../../data/star.mesh
   --order 1
   --no-static-condensation
   --refinements 7
   --serial-refinements 2
   --frequency 1
----------------------------------------------------------------------------------------
DOFs            h               L^2 error       L^2 rate        H^1 error       H^1 rate
----------------------------------------------------------------------------------------
31              0.4876          0.3252          0               2.631           0
101             0.2438          0.09293         1.807           1.387           0.9229
361             0.1219          0.02393         1.957           0.7017          0.9836
1361            0.06095         0.006027        1.989           0.3518          0.996
5281            0.03048         0.00151         1.997           0.176           0.999
20801           0.01524         0.0003776       1.999           0.08803         0.9997
82561           0.007619        9.441e-05       2               0.04402         0.9999
```

Note that the L2 error is converging at a rate of 2 while the H1 error is only converging at a rate of 1.

### Run 2 (High order)

Now consider the same run, only we are using 3rd order (cubic) basis functions instead.

```
./convergence -r 7 -o 3
Options used:
   --mesh ../../../data/star.mesh
   --order 3
   --no-static-condensation
   --refinements 7
   --serial-refinements 2
   --frequency 1
----------------------------------------------------------------------------------------
DOFs            h               L^2 error       L^2 rate        H^1 error       H^1 rate
----------------------------------------------------------------------------------------
211             0.4876          0.004777        0               0.118           0
781             0.2438          0.0003178       3.91            0.01576         2.905
3001            0.1219          2.008e-05       3.984           0.001995        2.982
11761           0.06095         1.258e-06       3.997           0.0002501       2.996
46561           0.03048         7.864e-08       4               3.129e-05       2.999
185281          0.01524         4.915e-09       4               3.912e-06       3
739201          0.007619        3.072e-10       4               4.891e-07       3
```

The L2 error is now converging at a rate of 4, and the H1 error is converging at a rate of 3.
This is because the exact solution in these runs is smooth, so higher-order methods
approximate it better.

#### Questions

{% include qanda
    question='How many unknowns do we need in runs 1 and 2 to get 4 digits of accuracy?'
    answer='We need only 3001 unknowns compared to 82561 unknowns for the low-order method!' %}

{% include qanda
    question='Which method is more efficient: low-order or high-order?'
    answer='The high-order method is more efficient.' %}

### Run 3 (3D example)
The previous two runs used a 2D mesh in serial, but the same code can be used to run a 3D problem in parallel.

```
mpirun -n 4 ./convergence -r 4 -o 2 -m ../../../data/inline-hex.mesh
Options used:
   --mesh ../../../data/inline-hex.mesh
   --order 2
   --no-static-condensation
   --refinements 4
   --serial-refinements 2
   --frequency 1
----------------------------------------------------------------------------------------
DOFs            h               L^2 error       L^2 rate        H^1 error       H^1 rate
----------------------------------------------------------------------------------------
729             0.25            0.001386        0               0.02215         0
4913            0.125           0.0001772       2.967           0.005532        2.002
35937           0.0625          2.227e-05       2.993           0.001377        2.007
274625          0.03125         2.787e-06       2.998           0.0003441       2
```

#### Questions

Experiment with different orders in 2D and 3D.

{% include qanda
    question='What convergence rate will you expect in L2 and H1 for a given basis order _p_?'
    answer='For a smooth exact solution, the convergence rate in energy norm (H1) is _p_.
    Using the so-called _Nitsche Trick_, one can prove that we pick an additional order in L2,
    so the convergence rate there is _p+1_' %}

---

## Out-Brief

We demonstrated the ease of implementing a order and dimension independent finite element
code in MFEM. We discussed the basics of the finite element method as well as demonstrated
the effect of the polynomial order of the basis functions on convergence rates.

### Further Reading

To learn more about MFEM, including many more [example codes](http://mfem.org/examples) and miniapps visit [mfem.org](http://mfem.org).

<!-- Insert space, horizontal line, and link to HandsOnLesson table -->

&nbsp;

---

[Back to all HandsOnLessons](../lessons.md)
