#include "Double.h"

bool
update_solution_ftcs(
    int n,                    // number of samples
    Double *curr,             // new array of samples to compute/return
    Double const *last,       // old/last array of samples computed
    Double alpha,             // thermal diffusivity
    Double dx, Double dt,     // spacing in space, x, and time, t.
    Double bc_0, Double bc_1) // boundary conditions @ x=0 & x=Lx
{
    Double r = alpha * dt / (dx * dx);

    if (r >= 0.5) return false;

    for (int i = 1; i < n-1; i++)
        curr[i] = r*last[i+1] + (1-2*r)*last[i] + r*last[i-1];

    curr[0] = bc_0;
    curr[n-1] = bc_1;

    return true;
}
