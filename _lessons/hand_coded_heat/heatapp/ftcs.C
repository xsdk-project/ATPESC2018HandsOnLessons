#include "Double.h"

bool
update_solution_ftcs(
    int n,                    // number of samples
    Double *uk1,              // new array of samples to compute/return
    Double const *uk0,        // old/last array of samples computed
    Double alpha,             // thermal diffusivity
    Double dx, Double dt,     // spacing in space, x, and time, t.
    Double bc_0, Double bc_1) // boundary conditions @ x=0 & x=Lx
{
    Double r = alpha * dt / (dx * dx);

    // Sanity check for stability
    if (r > 0.5) return false;

    for (int i = 1; i < n-1; i++)
        uk1[i] = r*uk0[i+1] + (1-2*r)*uk0[i] + r*uk0[i-1];

    // maintain boundary conditions
    uk1[0] = bc_0;
    uk1[n-1] = bc_1;

    return true;
}
