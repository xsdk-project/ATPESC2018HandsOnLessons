#include "Double.h"

void
update_solution_ftcs(int n, Double *curr, Double const *last,
    Double alpha, Double dx, Double dt,
    Double bc_0, Double bc_1)
{
    Double const r = alpha * dt / (dx * dx);

    // Update the solution using FTCS algorithm
    for (int i = 1; i < n-1; i++)
        curr[i] = r*last[i+1] + (1-2*r)*last[i] + r*last[i-1];

    // Impose boundary conditions for solution indices i==0 and i==n-1
    curr[0  ] = bc_0;
    curr[n-1] = bc_1;
}
