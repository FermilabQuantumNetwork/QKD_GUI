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

/* Function to fit interference to:
 *
 * y = A + B*cos(voltage^2*C + D) */
int cos_f(const gsl_vector *x, void *data_, gsl_vector *f)
{
    size_t n = ((struct data *)data_)->n;
    double *t = ((struct data *)data_)->t;
    double *y = ((struct data *)data_)->y;

    double A = gsl_vector_get(x, 0);
    double B = gsl_vector_get(x, 1);
    double C = gsl_vector_get(x, 2);
    /* FIXME: For now we fix A, B, and C to ensure we get reliable results. */
    A = 4.5;
    B = 0.5;
    C = 0.5;
    double D = gsl_vector_get(x, 3);

    size_t i;

    for (i = 0; i < n; i++) {
        double Yi = A + B*cos(t[i]*t[i]*C + D);
        gsl_vector_set(f, i, Yi - y[i]);
    }

    return GSL_SUCCESS;
}

void callback(const size_t iter, void *params, const gsl_multifit_nlinear_workspace *w)
{
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

    gsl_vector *f;
    gsl_matrix *J;
    gsl_matrix *covar = gsl_matrix_alloc(p, p);
    double t[N], y[N], weights[N];
    struct data d = { n, t, y };
    double x_init[4] = {0.0, 1.0, 1.0, 1.0}; /* starting values */
    gsl_vector_view x = gsl_vector_view_array(x_init, p);
    gsl_vector_view wts = gsl_vector_view_array(weights, n);
    gsl_rng * r;
    double chisq, chisq0;
    int status, info;
    size_t i;

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

    /* allocate workspace with default parameters */
    w = gsl_multifit_nlinear_alloc (T, &fdf_params, n, p);

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

        Log(VERBOSE, "A      = %.5f +/- %.5f", FIT(0), c*ERR(0));
        Log(VERBOSE, "B      = %.5f +/- %.5f", FIT(1), c*ERR(1));
        Log(VERBOSE, "C      = %.5f +/- %.5f", FIT(2), c*ERR(2));
        Log(VERBOSE, "D      = %.5f +/- %.5f", FIT(3), c*ERR(3));
    }

    double C = FIT(2);
    A = 4.5;
    B = 0.5;
    C = 0.5;
    double D = FIT(3);

    /* y = A + B*cos(voltage^2*C + D)
     *
     * Therefore, the minimum is at
     *     voltage^2*C + D = pi + 2*pi*n
     *     voltage^2*C     = pi + 2*pi*n - D
     *     voltage^2       = (pi + 2*pi*n - D)/C
     *     voltage         = sqrt((pi+2*pi*n - D)/C) */
    *min = sqrt((M_PI-D)/C);

    /* We want the minimum closest to 2.5 V. */
    int i;
    for (i = -10; i <= 10; i++) {
        double new_min = sqrt((M_PI+2*M_PI*n-D)/C);

        if (fabs(new_min-2.5) < fabs(*min - 2.5))
            *min = new_min;
    }

    Log(VERBOSE, "status = %s", gsl_strerror (status));

    gsl_multifit_nlinear_free (w);
    gsl_matrix_free (covar);
    gsl_rng_free (r);

    return 0;
}
