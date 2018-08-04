#include "heat.H"

bool
update_solution_ftcs(
    int n,                    // number of samples
    Double *uk1,              // new array of u(x,k+1) to compute/return
    Double const *uk0,        // old/last array u(x,k) of samples computed
    Double alpha,             // thermal diffusivity
    Double dx, Double dt,     // spacing in space, x, and time, t.
    Double bc0, Double bc1)   // boundary conditions @ x=0 & x=Lx
{
}
