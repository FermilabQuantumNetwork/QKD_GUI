#include <stdlib.h>
#include <stdio.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlinear.h>
#include <vector>

#define N      100    /* number of data points to fit */
#define TMAX   (3.0)  /* time variable in [0,TMAX] */

struct data {
    size_t n;
    double *t;
    double *y;
    /* Parameter bounds. */
    double *lo;
    double *hi;
    double *mu;
    double *sigma;
};

int cos_f(const gsl_vector *x, void *data, gsl_vector *f);
void callback(const size_t iter, void *params, const gsl_multifit_nlinear_workspace *w);
int fit(std::vector<double> *x, std::vector<double> *y, double *min);
