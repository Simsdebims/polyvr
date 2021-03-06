#include "equation.h"
#include <math.h>
#include <iostream>

using namespace OSG;

equation::equation(double a, double b, double c, double d) : a(a), b(b), c(c), d(d) {;}

int equation::solve(double& x1, double& x2, double& x3) {
    if (a == 0) {
        if (b == 0) { // linear equation
            if (c == 0) return 0;
            x1 = -d/c;
            return 1;
        }

        // quadratic
        double delta = (c*c-4*b*d);
        double inv_2a = 1.0/(2*b);
        if (delta >= 0) {
            double root = sqrt(delta);
            x1 = (-c-root)*inv_2a;
            x2 = (-c+root)*inv_2a;
            return 2;
        } else return 0;
    }

    // cubic (TODO: reuse from driving simulator asphalt shader)
    return 0;
}
