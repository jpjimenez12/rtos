#include "process.h"            // Library to manage the process

void function(void *arg)
{
    double term = 1.0, sum = 1.0;   // Inicializa las variables en x = 1
    int nterm = (int) arg;
    nterm *= WORKUNIT;
    int yield = 0;
    for(int i = 1; i < nterm; i++)
    {   
        proc_progress(i);   // Compute the progress of the process
        term *= (4.0*i*i-4.0*i+1.0)/(4.0*i*i+2.0*i);
        sum += term;
        if(i%15 == 0)
        {
            proc_yield();
        }
    }
    
    double result = 2.0*sum;    // Store the result of the current process
    
    printf("Result of the process %d usign %d terms: %lf\n", proc_id(), nterm,  result);
    proc_result(result);        // store the final result

    proc_finished();
}


int main(int argc, char **argv)
{   
    int Total_proc = 25;
    int burst[Total_proc];
    srand(time(NULL));
    for(int i = 0; i < Total_proc; i++)
    {
        burst[i] = rand()%1000;
    }

    // Creating all the processes
    Process *proc1 = proc_create(function, (void *) burst[0]);
    for(int i = 1; i < Total_proc; i++)
    {
        proc_create(function, burst[i]);
    }

    // Run the scheduler
    proc_join(proc1);

    return 0;
}
