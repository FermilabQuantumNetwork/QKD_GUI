#include <stdlib.h>
#include <stdio.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlinear.h>
#include <vector>

#define N      1000    /* maximum number of data points to fit */

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
int fit(std::vector<double> *v, std::vector<double> *qber, std::vector<double> *std_qber, double *min);
