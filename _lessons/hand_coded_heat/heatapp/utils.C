#if 0
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <cassert>
#include <cmath>

#include "Double.h"

extern int Nx;
extern Double *exact;
extern char const *probnm;
extern int noout;

// Utilities
Double
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

void
copy(int n, Double *dst, Double const *src)
{
    int i;
    for (i = 0; i < n; i++)
        dst[i] = src[i];
}

void
write_array(int t, int n, Double dx, Double const *a)
{
    int i;
    char fname[32];
    char vname[64];
    FILE *outf;

    if (noout) return;

    if (t == TSTART)
    {
        snprintf(fname, sizeof(fname), "%s/%s_soln_00000.curve", probnm, probnm);
        snprintf(vname, sizeof(vname), "temperature");
    }
    else if (t == TFINAL)
    {
        snprintf(fname, sizeof(fname), "%s/%s_soln_final.curve", probnm, probnm);
        snprintf(vname, sizeof(vname), "temperature");
    }
    else if (t == RESIDUAL)
    {
        snprintf(fname, sizeof(fname), "%s/%s_change.curve", probnm, probnm);
        snprintf(vname, sizeof(vname), "%s/%s_l2_change", probnm, probnm);
    }
    else if (t == ERROR)
    {
        snprintf(fname, sizeof(fname), "%s/%s_error.curve", probnm, probnm);
        snprintf(vname, sizeof(vname), "%s/%s_l2", probnm, probnm);
    }
    else
    {
        if (a == exact)
        {
            snprintf(fname, sizeof(fname), "%s/%s_exact_%05d.curve", probnm, probnm, t);
            snprintf(vname, sizeof(vname), "exact_temperature");
        } 
        else
        {
            snprintf(fname, sizeof(fname), "%s/%s_soln_%05d.curve", probnm, probnm, t);
            snprintf(vname, sizeof(vname), "temperature");
        }
    }


    outf = fopen(fname,"w");
    fprintf(outf, "# %s\n", vname);
    for (i = 0; i < n; i++)
        fprintf(outf, "%8.4g %8.4g\n", i*((double)dx), (double) a[i]);
    fclose(outf);
}

void
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
