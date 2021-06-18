#include <stdio.h>
#include <stdlib.h>
#include "process.h"


// Pointers to implement the queue
struct Queue *head;     // Pointer to the process on the head of the queue
struct Queue *tail;     // Pointer to the process on the tail of the queue
struct Queue *current;  // Pointer to the currente running process

// Context variable
jmp_buf scheduler;       // Variable to store the main context
jmp_buf *running;   // Pointer to the context of the running process

// Global Variables
static int t_count = 0;         // Tickets counte
static int process_count = 0;   // Number of active processes
int Tail_flag = TRUE;           // Flag to know if the process is the first tail proc
static int finished = 0;        // ID of the last process finished
int quantum = 3;                // Value of time for each process to run in the processor


// Create a process and add it to the RR queue
Process *proc_create(void (*function)(void*), void *arg)
{   
    jmp_buf *context = malloc(sizeof(jmp_buf)); // Memory allocation for the context
    struct Process *proc = malloc(sizeof(Process)); // Memory allocation for the process
    context_create(context, function, (void *) arg); // Create the context of the process
    proc->context = context;    // Store the context in process struct
    proc->id = ++process_count;   // Asign the ID to the process
    proc->state = READY;        // Set the process ready to run
    proc->burst = (int) arg;    // store the burst
    // Loop to assign the tickets
    for(int i = 0; i < (int) arg; i++)
    {
        proc->tickets[i] = ++t_count;
    }

    if(head==NULL)  // if the list is empty add the process first
    {
        struct Queue *first = malloc(sizeof(struct Queue)); // Memory allocation of the first process
        first->proc = proc;     // Save the process as the current process
        first->nextproc = NULL; // There's no next process
        first->prevproc = NULL; // There's no previous process
        head = first;   // Set the head to first process
        tail = first;   // Also set the tail to first process
        //free(first);    // Free the memory related to first
    }
    else    // The queue isn't empty so add the new process
    {
        struct Queue *newp = malloc(sizeof(struct Queue));
        newp->prevproc = tail;  // Store the tail as prev process
        newp->prevproc->nextproc = newp;    // Set the process as the next tail process
        newp->proc = proc;      // Set the process as current
        newp->nextproc = head;  // Set the head as next process
        head->prevproc = newp;  // Set the proc as head prev proc
        if(Tail_flag == TRUE)
        {
            head->nextproc = newp;
            Tail_flag = FALSE;  // Fisrt tail process was set
        }
        tail = newp;    // Store the new proc in the tail
        //free(newp);     // Free the memory used by newp
    }
    return proc;    // Return the process created

}

// Obtain the id of the process
int proc_id(void)
{
    return current->proc->id;
}

// Store the result of the process
void proc_result(double result)
{
    current->proc->result = result;
}

// Compute the progress of the current process
void proc_progress(int current_term)
{
    int total = current->proc->burst;
    total *= WORKUNIT;
    float partial = (float) current_term/(float) total;
    current->proc->progress = partial*100;
}

float progress()
{
    return current->proc->progress;
}


// Swith the control to the scheduler
void proc_yield(void)
{
    context_switch(running, &scheduler);
}

// Remove the process from the queue
void proc_remove(void)
{
    Queue *temp = current->nextproc;    // Store the next process
    current->prevproc->nextproc = current->nextproc;    // Update the prev proc
    current->nextproc->prevproc = current->prevproc;    // Update the next proc
    free(current->proc->context);
    free(current->proc);
    free(current);
    current = temp;                 // Set the next process as current
}

// Finish the process
void proc_finished(void)
{
    finished++;
    current->proc->state = FINISHED;        // Set the status of the process as finished
    //update_tickets();
    context_switch(running, &scheduler);         // Return the control to the scheduler
}

// Scheduler
void proc_join(Process *proc)
{
    current = head;     // Set the process in the head as current
    time_t now;         // Current time in clock cycles

    while(TRUE)
    {   
        if(finished == process_count)
        {
            break; // Process process is finished so break
        }

        struct sigaction action;        // Struct to manage the signals
        action.sa_handler = proc_yield; // The signal handler is the process_yield function
        action.sa_flags = 0;            // NO flags needed
        sigfillset(&action.sa_mask);
        sigaction(SIGALRM, &action, NULL);

        struct itimerval timer;             // Create a timer from sys
        timer.it_value.tv_sec = quantum;    // Usable time by every process
        timer.it_value.tv_usec = 0;
        timer.it_interval = timer.it_value;
        setitimer(ITIMER_REAL, &timer, NULL);

        if(current->proc->state == READY)
        {
            running = current->proc->context;
            context_switch(&scheduler, running);     // Run the current process
        }


        //********* add the lottery ticket function *********//

        Lottery();  // Do the lottery

        //*************************************************//
        //current = current->nextproc;    // When the prev proc end running start the next one
        
    }
}


void Lottery()
{
    int winner = rand() % t_count;
    struct Queue *temp = malloc(sizeof(Queue));
    temp = head;

    //printf("The winner is: %d\n", winner);

    for(int j = 0; j < process_count; j++)
    {
        for(int i = 0; i < temp->proc->burst; i++)
        {
            if(temp->proc->tickets[i] == winner)
            {
                //printf("The process %d is the winner\n", proc_id());
                current = temp;
                break;
            }
        }
        temp = temp->nextproc;
    }
}

/*
void update_tickets()
{
    t_count = 0;
    struct Queue *temp = malloc(sizeof(Queue));
    temp = current;
    for(int i=0; i<process_count; i++)
    {
        if(temp->proc->state != FINISHED)
        {
            for(int i=0; i<temp->proc->burst; i++)
            {
                temp->proc->tickets[i] = ++t_count;
            }
        }
        temp = temp->nextproc;
    }
}
*/