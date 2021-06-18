//#include <gtk/gtk.h>
#include "process.h"            // Library to manage the process

static yield;

void function(void *arg)
{
    double term = 1.0, sum = 1.0;   // Inicializa las variables en x = 1
    int nterm = (int) arg;
    nterm *= WORKUNIT;
    for(int i = 1; i < nterm; i++)
    {   
        proc_progress(i);   // Compute the progress of the process
        term *= (4.0*i*i-4.0*i+1.0)/(4.0*i*i+2.0*i);
        sum += term;
        if(i%yield == 0)
        {
            proc_yield();   // give the processor to other process
        }
    }
    
    double result = 2.0*sum;    // Store the result of the current process
    
    printf("Result of the process %d usign %d terms: %lf\n", proc_id(), nterm,  result);
    proc_result(result);        // store the final result

    proc_finished();
}

/*
static void activate (GtkApplication* app, gpointer user_data)
{
    GtkWidget *window;

    //Create a window with a title, and a default size
    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW(window), "Lottery Scheduler");
    gtk_window_set_default_size(GTK_WINDOW(window), 640, 480);
    
    gtk_widget_show_all (window);
}
*/

int main(int argc, char **argv)
{   
    /*
    // Starting GTK
    GtkApplication *app;
    int status;
    

    app = gtk_application_new(NULL, G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    */

    int Total_proc = 10;
    int burst[Total_proc];
    yield = 5;
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

    //return status;
    return 0;
}
