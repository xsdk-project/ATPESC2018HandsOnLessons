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

#define HANDLE_ARG(VAR, TYPE, STYLE, HELP) \
{ \
    char const *style = #STYLE; \
    char const *q = style[1]=='s'?"\"":""; \
    void *valp = (void*) &VAR; \
    int const len = strlen(#VAR)+1; \
    std::stringstream strmvar; \
    for (i = 1; i < argc; i++) \
    {\
        int valid_style = style[1]=='d'||style[1]=='g'||style[1]=='s'; \
        if (strncmp(argv[i], #VAR"=", len)) \
            continue; \
        assert(valid_style); \
	if (strlen(argv[i]+len)) \
        {\
            if      (style[1] == 'd') /* int */ \
                *((int*) valp) = (int) strtol(argv[i]+len,0,10); \
            else if (style[1] == 'g') /* double */ \
                *((Double*) valp) = (double) strtod(argv[i]+len,0); \
            else if (style[1] == 's') /* char* */ \
                *((char**) valp) = (char*) strdup(argv[i]+len); \
        }\
    }\
    strmvar << VAR; \
    if (help) \
    {\
        char tmp[256]; \
        int len = snprintf(tmp, sizeof(tmp), "        %s=%s%s%s", \
            #VAR, q, strmvar.str().c_str(), q);\
        snprintf(tmp, sizeof(tmp), "%s (%s)", #HELP, #TYPE); \
        fprintf(stderr, "        %s=%s%s%s%*s\n", \
            #VAR, q, strmvar.str().c_str(), q, 80-len, tmp);\
    }\
    else \
        fprintf(stderr, "    %s=%s%s%s\n", \
            #VAR, q, strmvar.str().c_str(), q);\
}

static void
process_args(int argc, char **argv)
{
    int i;
    int help = 0;

    // quick pass for 'help' anywhere on command line
    for (i = 0; i < argc && !help; i++)
        help = 0!=strcasestr(argv[i], "help");
    
    if (help)
    {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "    ./heat <arg>=<value> <arg>=<value>...\n");
    }

    HANDLE_ARG(prec, char*, %s, precision half|float|double|quad);
    HANDLE_ARG(alpha, double, %g, material thermal diffusivity);
    HANDLE_ARG(lenx, double, %g, material length);
    HANDLE_ARG(dx, double, %g, x-incriment (best if lenx/dx->int));
    HANDLE_ARG(dt, double, %g, t-incriment);
    HANDLE_ARG(maxt, double, %g, max. time of simulation or set negative change threshold);
    HANDLE_ARG(bc0, double, %g, boundary condition @ x=0: u(0,t));
    HANDLE_ARG(bc1, double, %g, boundary condition @ x=1: u(1,t));
    HANDLE_ARG(ic, char*, %s, initial condition @ t=0: u(x,0));
    HANDLE_ARG(alg, char*, %s, algorithm ftcs|upwind15|crankn);
    HANDLE_ARG(savi, int, %d, save every i-th solution step);
    HANDLE_ARG(save, int, %d, save error in every saved solution);
    HANDLE_ARG(outi, int, %d, output progress every i-th solution step);
    HANDLE_ARG(noout, int, %d, disable all file outputs);

    if (help)
    {
        fprintf(stderr, "Examples...\n");
        fprintf(stderr, "    ./heat dx=0.01 dt=0.0002 alg=ftcs\n");
        fprintf(stderr, "    ./heat dx=0.1 bc0=5 bc1=10 ic=\"spikes(5,5)\"\n");
        exit(1);
    }

    // Handle possible termination by change threshold criterion
    if (maxt < 0)
    {
        min_change = -maxt * -maxt;
        maxt = INT_MAX;
    }
}
