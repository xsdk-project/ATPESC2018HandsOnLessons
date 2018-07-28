#include <unistd.h>

#include <cassert>
#ifdef HAVE_FEENABLEEXCEPT
#define _GNU_SOURCE
#include <cfenv>
#endif
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sstream>

#include "Double.h"

// Double class' statics
int         Double::nadds  = 0;
int         Double::nmults = 0;
int         Double::ndivs  = 0;
std::size_t Double::nbytes = 0;

// Command-line argument variables
int noout        = 0;
int savi         = 0;
int outi         = 100;
int save         = 0;
char const *probnm = "heat";
char const *alg  = "ftcs";
char const *prec = "double";
char const *ic   = "const(1)";
Double lenx      = 1.0;
Double alpha     = 0.2;
Double dt        = 0.004;
Double dx        = 0.1;
Double bc0       = 0;
Double bc1       = 1;
Double maxt      = 2.0;
Double min_change = 1e-8*1e-8;

// Various arrays of numerical data
Double *curr           = 0; // current solution
Double *last           = 0; // last solution
Double *exact          = 0; // exact solution (when available)
Double *change_history = 0; // solution l2norm change history
Double *error_history  = 0; // solution error history (when available)
Double *cn_Amat        = 0; // A matrix for Crank-Nicholson

// Number of points in space, x, and time, t.
int Nx = (int) (lenx/dx);
int Nt = (int) (maxt/dt);

// Utilities
static Double
l2_norm(int n, Double const *a, Double const *b)
{
    int i;
    Double sum = 0;
    for (i = 0; i < n; i++)
    {
        Double diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sum / n;
}

static void
copy(int n, Double *dst, Double const *src)
{
    int i;
    for (i = 0; i < n; i++)
        dst[i] = src[i];
}

#define TSTART -1
#define TFINAL -2
#define RESIDUAL -3
#define ERROR -4
static void
write_array(int t, int n, Double dx, Double const *a)
{
    int i;
    char fname[32];
    FILE *outf;

    if (noout) return;

    if (t == TSTART)
        snprintf(fname, sizeof(fname), "heat_soln_00000.curve");
    else if (t == TFINAL)
        snprintf(fname, sizeof(fname), "heat_soln_final.curve");
    else if (t == RESIDUAL)
        snprintf(fname, sizeof(fname), "change.curve");
    else if (t == ERROR)
        snprintf(fname, sizeof(fname), "error.curve");
    else
    {
        if (a == exact)
            snprintf(fname, sizeof(fname), "heat_exact_%05d.curve", t);
        else
            snprintf(fname, sizeof(fname), "heat_soln_%05d.curve", t);
    }
    
    outf = fopen(fname,"w");
    for (i = 0; i < n; i++)
        fprintf(outf, "%8.4g %8.4g\n", i*((double)dx), (double) a[i]);
    fclose(outf);
}

static void
set_initial_condition(int n, Double *a, Double dx, char const *ic)
{
    int i;
    Double x;

    if (!strncmp(ic, "const(", 6)) /* const(val) */
    {
        double cval = strtod(ic+6, 0);
        for (i = 0; i < n; i++)
            a[i] = cval;
    }
    else if (!strncmp(ic, "step(", 5)) /* step(left,xmid,right) */
    {
        char *p;
        double left = strtod(ic+5, &p);
        double xmid = strtod(p+1, &p);
        double right = strtod(p+1, 0);
        for (i = 0, x = 0; i < n; i++, x+=dx)
        {
            if (x < xmid) a[i] = left;
            else          a[i] = right;
        }
    }
    else if (!strncmp(ic, "ramp(", 5)) /* ramp(left,right) */
    {
        char *p;
        double left = strtod(ic+5, &p);
        double right = strtod(p+1, 0);
        double dv = (right-left)/(n-1);
        for (i = 0, x = left; i < n; i++, x+=dv)
            a[i] = x;
    }
    else if (!strncmp(ic, "rand(", 5)) /* rand(seed,amp) */
    {
        char *p;
        int seed = (int) strtol(ic+5,&p,10);
        double amp = strtod(p+1, 0);
        const double maxr = ((long long)1<<31)-1;
        srandom(seed);
        for (i = 0; i < n; i++)
            a[i] = amp * random()/maxr;
    }
    else if (!strncmp(ic, "sin(Pi*x)", 9)) /* rand(seed,amp) */
    {
        for (i = 0, x = 0; i < n; i++, x+=dx)
            a[i] = sin(M_PI*x);
    }
    else if (!strncmp(ic, "spikes(", 7)) /* spikes(Const,Amp,Loc,Amp,Loc,...) */
    {
        char *next;
        double cval = strtod(ic+7, &next);
        char const *p = next;
        for (i = 0, x = 0; i < n; i++)
            a[i] = cval;
        while (*p != ')')
        {
            char *ep_amp, *ep_idx;
            double amp = strtod(p+1, &ep_amp);
            int idx = (int) strtod(ep_amp+1, &ep_idx);
            assert(idx<n);
            a[idx] = amp;
            p = ep_idx;
        }

    }

    write_array(TSTART, Nx, dx, a);
}

extern void
initialize_crankn(int n,
    Double alpha, Double dx, Double dt,
    Double **_cn_Amat);

static void
initialize(void)
{
    Nx = (int) (lenx/dx);
    Nt = (int) (maxt/dt);
    dx = lenx/(Nx-1);

    curr = new Double[Nx]();
    last = new Double[Nx]();
    if (save)
    {
        exact = new Double[Nx]();
        change_history = new Double[Nx]();
        error_history = new Double[Nx]();
    }

    assert(strncmp(alg, "ftcs", 4)==0 ||
           strncmp(alg, "upwind15", 8)==0 ||
           strncmp(alg, "crankn", 6)==0);

#ifdef HAVE_FEENABLEEXCEPT
    feenableexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW);
#endif

    if (!strncmp(alg, "crankn", 6))
        initialize_crankn(Nx, alpha, dx, dt, &cn_Amat);

    /* Initial condition */
    set_initial_condition(Nx, last, dx, ic);
}

extern void
process_args(int argc, char **argv);

static void 
compute_exact_solution(int n, Double *a, Double dx, char const *ic,
    Double alpha, Double t, Double bc0, Double bc1)
{
    int i;
    Double x;
    
    if (bc0 == 0 && bc1 == 0 && !strncmp(ic, "sin(Pi*x)", 9))
    {
        for (i = 0, x = 0; i < n; i++, x+=dx)
            a[i] = sin(M_PI*x)*exp(-alpha*M_PI*M_PI*t);
    }
    else if (bc0 == 0 && bc1 == 0 && !strncmp(ic, "const(", 6))
    {
        Double cval = strtod(ic+6, 0);
        for (i = 0, x = 0; i < n; i++, x+=dx)
        {
            int n;
            Double fsum = 0;

            // sum first 200 terms of Fourier series
            for (n = 1; n < 200; n++)
            {
                Double coeff = 2*cval*(1-pow(-1.0,(double)n))/(n*M_PI);
                Double func = sin(n*M_PI*x)*exp(((double)-alpha)*n*n*M_PI*M_PI*((double)t));
                fsum += coeff * func;
            }
            a[i] = fsum;
        }
    }
    else // can only compute final steady state solution
    {
        for (i = 0, x = 0; i < n; i++, x+=dx)
            a[i] = bc0 + (bc1-bc0)*x;
    }
}

extern void
update_solution_ftcs(int n,
    Double *curr, Double const *last,
    Double alpha, Double dx, Double dt,
    Double bc_0, Double bc_1);

extern void
update_solution_upwind15(int n,
    Double *curr, Double const *last,
    Double alpha, Double dx, Double dt,
    Double bc_0, Double bc_1);

extern void
update_solution_crankn(int n,
    Double *curr, Double const *last,
    Double const *cn_Amat,
    Double bc_0, Double bc_1);

int finalize(int ti, Double maxt, Double change)
{
    int retval = 0;

    write_array(TFINAL, Nx, dx, curr);
    if (save)
    {
        write_array(RESIDUAL, ti, dt, change_history);
        write_array(ERROR, ti, dt, error_history);
    }

    if (outi)
    {
        printf("Iteration %04d: last change l2=%g\n", ti, (double) change);
        printf("Counts: %s\n", Double::counts_string());
    }

    delete [] curr;
    delete [] last;
    if (exact) delete [] exact;
    if (change_history) delete [] change_history;
    if (error_history) delete [] error_history;
    if (cn_Amat) delete [] cn_Amat;
    if (strncmp(alg, "ftcs", 4)) free((void*)alg);
    if (strncmp(prec, "double", 6)) free((void*)prec);
    if (strncmp(ic, "const(1)", 8)) free((void*)ic);

    return retval;
}

static void
update_solution()
{
    if (!strcmp(alg, "ftcs"))
        update_solution_ftcs(Nx, curr, last, alpha, dx, dt, bc0, bc1);
    else if (!strcmp(alg, "upwind15"))
        update_solution_upwind15(Nx, curr, last, alpha, dx, dt, bc0, bc1);
    else if (!strcmp(alg, "crankn"))
        update_solution_crankn(Nx, curr, last, cn_Amat, bc0, bc1);
}

static Double
update_output_files(int ti)
{
    Double change;

    if (ti>0 && save)
    {
        compute_exact_solution(Nx, exact, dx, ic, alpha, ti*dt, bc0, bc1);
        if (savi && ti%savi==0)
            write_array(ti, Nx, dx, exact);
    }

    if (ti>0 && savi && ti%savi==0)
        write_array(ti, Nx, dx, curr);

    change = l2_norm(Nx, curr, last);
    if (save)
    {
        change_history[ti] = change;
        error_history[ti] = l2_norm(Nx, curr, exact);
    }

    return change;
}

int main(int argc, char **argv)
{
    int ti;
    Double change;

    // Read command-line args and set values
    process_args(argc, argv);

    // Allocate arrays and set initial conditions
    initialize();

    // Iterate to max iterations or solution change is below threshold
    for (ti = 0; ti*dt < maxt; ti++)
    {
        // compute the next solution step
        update_solution();

        // compute amount of change in solution
        change = update_output_files(ti);

        // Handle possible termination by change threshold
        if (maxt == INT_MAX && change < min_change)
        {
            printf("Stopped after %06d iterations for threshold %g\n",
                ti, (double) change);
            break;
        }

        // Output progress
        if (outi && ti%outi==0)
            printf("Iteration %04d: last change l2=%g\n", ti, (double) change);

        // Copy current solution to last
        copy(Nx, last, curr);
    }

    // Delete storage and output final results
    return finalize(ti, maxt, change);
}
