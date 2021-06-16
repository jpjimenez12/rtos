#include "process.h"            // Library to manage the process

#define WORKUNIT 50

void *function(int burst, double *result)
{
    double term = 1.0, sum = 1.0;   // Inicializa las variables en x = 1
    int nterm = burst*WORKUNIT;
    for(int i = 1; i < nterm; i++)
    {
        term *= (4.0*i*i-4.0*i+1.0)/(4.0*i*i+2.0*i);
        sum += term;
    }
    
    *result = 2.0*sum;

    printf("The Value of pi is: %lf\n", *result);
}

int main(int argc, char **argv)
{
    double result = 0.0;
    function(10, &result);

    printf("In the main result is also %lf\n", result);
    return 0;
}
