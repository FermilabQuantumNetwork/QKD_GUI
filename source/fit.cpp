#include <stdlib.h>
#include <stdio.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlinear.h>
#include <vector>
#include "fit.h"
#include "logging.h"

#define UNUSED(V) ((void) V)

double map_parameter_ext_to_int(double ext, double lo, double hi)
{
    return asin(2*((ext-lo)/(hi-lo))-1);
}

double map_parameter_int_to_ext(double x, double lo, double hi)
{
    return lo + (hi-lo)*(sin(x)+1)/2;
}

double map_error_int_to_ext(double x, double err, double lo, double hi)
{
    return err*fabs((hi-lo)*cos(x)/2);
}

/* Function to fit interference to:
 *
 * y = A + B*cos(voltage^2*C + D) */
int cos_f(const gsl_vector *x, void *data_, gsl_vector *f)
{
    double A, B, C, D;
    size_t n = ((struct data *)data_)->n;
    double *t = ((struct data *)data_)->t;
    double *y = ((struct data *)data_)->y;
    double *lo = ((struct data *)data_)->lo;
    double *hi = ((struct data *)data_)->hi;

    A = map_parameter_int_to_ext(gsl_vector_get(x, 0),lo[0],hi[0]);
    B = map_parameter_int_to_ext(gsl_vector_get(x, 1),lo[1],hi[1]);
    C = map_parameter_int_to_ext(gsl_vector_get(x, 2),lo[2],hi[2]);
    D = map_parameter_int_to_ext(gsl_vector_get(x, 3),lo[3],hi[3]);

    size_t i;

    for (i = 0; i < n; i++) {
        double Yi = A + B*cos(t[i]*t[i]*C + D);
        gsl_vector_set(f, i, Yi - y[i]);
    }

    /* Apply priors. */
    for (i = 0; i < 4; i++) {
        gsl_vector_set(f, n+i, (map_parameter_int_to_ext(gsl_vector_get(x, i),lo[i],hi[i])-mu[i])/sigma[i]);
    }

    return GSL_SUCCESS;
}

void callback(const size_t iter, void *params, const gsl_multifit_nlinear_workspace *w)
{
    UNUSED(params);
    gsl_vector *f = gsl_multifit_nlinear_residual(w);
    gsl_vector *x = gsl_multifit_nlinear_position(w);
    double rcond;

    /* compute reciprocal condition number of J(x) */
    gsl_multifit_nlinear_rcond(&rcond, w);

    Log(VERBOSE, "iter %2zu: A = %.4f, B = %.4f, C = %.4f, D = %.4f, cond(J) = %8.4f, |f(x)| = %.4f",
          iter,
          gsl_vector_get(x, 0),
          gsl_vector_get(x, 1),
          gsl_vector_get(x, 2),
          gsl_vector_get(x, 3),
          1.0 / rcond,
          gsl_blas_dnrm2(f));
}

int fit(std::vector<double> *v, std::vector<double> *qber, double *min)
{
    const gsl_multifit_nlinear_type *T = gsl_multifit_nlinear_trust;
    gsl_multifit_nlinear_workspace *w;
    gsl_multifit_nlinear_fdf fdf;
    gsl_multifit_nlinear_parameters fdf_params = gsl_multifit_nlinear_default_parameters();
    const size_t n = v->size();
    const size_t p = 4;
    static double prev_phase = -100;

    gsl_vector *f;
    gsl_matrix *J;
    gsl_matrix *covar = gsl_matrix_alloc(p, p);
    double t[N], y[N], weights[N], lo[100], hi[100], mu[100], sigma[100];
    struct data d = { n, t, y, lo, hi, mu, sigma };
    double x_init[4] = {0.5, 0.5, 1.0, 1.0}; /* starting values */
    if (prev_phase > 0)
        x_init[3] = prev_phase;
    gsl_vector_view x = gsl_vector_view_array(x_init, p);
    gsl_vector_view wts = gsl_vector_view_array(weights, n+4);
    gsl_rng * r;
    double chisq, chisq0;
    int status, info;
    int i;

    const double xtol = 1e-8;
    const double gtol = 1e-8;
    const double ftol = 0.0;

    gsl_rng_env_setup();
    r = gsl_rng_alloc(gsl_rng_default);

    /* define the function to be minimized */
    fdf.f = cos_f;
    fdf.df = NULL;   /* set to NULL for finite-difference Jacobian */
    fdf.fvv = NULL;     /* not using geodesic acceleration */
    fdf.n = n;
    fdf.p = p;
    fdf.params = &d;

    /* Set parameter bounds. */
    lo[0] = 0.1;
    hi[0] = 1.0;
    lo[1] = 0.1;
    hi[1] = 1.0;
    lo[2] = 0.1;
    hi[2] = 10.0;
    lo[3] = 0;
    hi[3] = 2*M_PI;

    /* Set priors. */
    mu[0] = 0.5;
    sigma[0] = 0.1;
    mu[1] = 0.5;
    sigma[1] = 0.1;
    mu[2] = 1.0;
    sigma[2] = 1.0;
    mu[3] = 1.0;
    sigma[3] = 100.0;

    /* Map initial guess to internal parameter values. */
    for (i = 0; i < p; i++)
        x_init[i] = map_parameter_ext_to_int(x_init[i],lo[i],hi[i]);

    /* this is the data to be fitted */
    for (i = 0; i < n; i++)
    {
        double ti = (*v)[i];
        double yi = (*qber)[i];
        /* FIXME: add weights later. */
        //double si = 0.1 * yi;

        t[i] = ti;
        y[i] = yi;
        weights[i] = 1.0;// / (si * si);
    };

    for (i = 0; i < p; i++)
    {
        t[n+i] = 0;
        y[n+i] = 0;
        weights[n+i] = 1.0;
    };

    /* allocate workspace with default parameters */
    w = gsl_multifit_nlinear_alloc (T, &fdf_params, n+p, p);

    /* initialize solver with starting point and weights */
    gsl_multifit_nlinear_winit (&x.vector, &wts.vector, &fdf, w);

    /* compute initial cost function */
    f = gsl_multifit_nlinear_residual(w);
    gsl_blas_ddot(f, f, &chisq0);

    /* solve the system with a maximum of 100 iterations */
    status = gsl_multifit_nlinear_driver(100, xtol, gtol, ftol, callback, NULL, &info, w);

    /* compute covariance of best fit parameters */
    J = gsl_multifit_nlinear_jac(w);
    gsl_multifit_nlinear_covar (J, 0.0, covar);

    /* compute final cost */
    gsl_blas_ddot(f, f, &chisq);

    #define FIT(i) gsl_vector_get(w->x, i)
    #define ERR(i) sqrt(gsl_matrix_get(covar,i,i))

    Log(VERBOSE, "summary from method '%s/%s'",
          gsl_multifit_nlinear_name(w),
          gsl_multifit_nlinear_trs_name(w));
    Log(VERBOSE, "number of iterations: %zu",
          gsl_multifit_nlinear_niter(w));
    Log(VERBOSE, "function evaluations: %zu", fdf.nevalf);
    Log(VERBOSE, "Jacobian evaluations: %zu", fdf.nevaldf);
    Log(VERBOSE, "reason for stopping: %s",
          (info == 1) ? "small step size" : "small gradient");
    Log(VERBOSE, "initial |f(x)| = %f", sqrt(chisq0));
    Log(VERBOSE, "final   |f(x)| = %f", sqrt(chisq));

    {
        double dof = n - p;
        double c = GSL_MAX_DBL(1, sqrt(chisq / dof));

        Log(VERBOSE, "chisq/dof = %g", chisq / dof);

        Log(VERBOSE, "Internal");
        Log(VERBOSE, "A      = %.5f +/- %.5f", FIT(0), c*ERR(0));
        Log(VERBOSE, "B      = %.5f +/- %.5f", FIT(1), c*ERR(1));
        Log(VERBOSE, "C      = %.5f +/- %.5f", FIT(2), c*ERR(2));
        Log(VERBOSE, "D      = %.5f +/- %.5f", FIT(3), c*ERR(3));
        Log(VERBOSE, "External");
        Log(VERBOSE, "A      = %.5f +/- %.5f", map_parameter_int_to_ext(FIT(0),lo[0],hi[0]), c*map_error_int_to_ext(FIT(0),ERR(0),lo[0],hi[0]));
        Log(VERBOSE, "B      = %.5f +/- %.5f", map_parameter_int_to_ext(FIT(1),lo[1],hi[1]), c*map_error_int_to_ext(FIT(1),ERR(1),lo[1],hi[1]));
        Log(VERBOSE, "C      = %.5f +/- %.5f", map_parameter_int_to_ext(FIT(2),lo[2],hi[2]), c*map_error_int_to_ext(FIT(2),ERR(2),lo[2],hi[2]));
        Log(VERBOSE, "D      = %.5f +/- %.5f", map_parameter_int_to_ext(FIT(3),lo[3],hi[3]), c*map_error_int_to_ext(FIT(3),ERR(3),lo[3],hi[3]));
    }

    //double A = map_parameter_int_to_ext(FIT(0),lo[0],hi[0]);
    //double B = map_parameter_int_to_ext(FIT(1),lo[1],hi[1]);
    double C = map_parameter_int_to_ext(FIT(2),lo[2],hi[2]);
    double D = map_parameter_int_to_ext(FIT(3),lo[3],hi[3]);

    prev_phase = D;

    /* y = A + B*cos(voltage^2*C + D)
     *
     * Therefore, the minimum is at
     *     voltage^2*C + D = pi + 2*pi*n
     *     voltage^2*C     = pi + 2*pi*n - D
     *     voltage^2       = (pi + 2*pi*n - D)/C
     *     voltage         = sqrt((pi+2*pi*n - D)/C) */
    *min = -1;

    /* We want the minimum closest to 2.5 V. */
    for (i = -10; i <= 10; i++) {
        if ((M_PI+2*M_PI*i-D)/C < 0)
            continue;

        double new_min = sqrt((M_PI+2*M_PI*i-D)/C);

        if ((*min < 0) || (fabs(new_min-2.5) < fabs(*min - 2.5)))
            *min = new_min;
    }

    if (*min < 0) {
        Log(WARNING, "unable to find a valid minimum! setting voltage to zero.");
        *min = 0;
    }

    Log(VERBOSE, "status = %s", gsl_strerror (status));

    gsl_multifit_nlinear_free (w);
    gsl_matrix_free (covar);
    gsl_rng_free (r);

    return 0;
}
