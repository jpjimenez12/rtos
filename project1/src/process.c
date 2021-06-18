#include <stdio.h>
#include <stdlib.h>
#include "process.h"


// Pointers to implement the queue
struct Queue *head;     // Pointer to the process on the head of the queue
struct Queue *tail;     // Pointer to the process on the tail of the queue
struct Queue *current;  // Pointer to the currente running process

// Context variable
jmp_buf main;       // Variable to store the main context
jmp_buf *running;   // Pointer to the context of the running process

// Global Variables
int process_count = 0;  // Number of active processes
int Tail_flag = TRUE;   // Flag to know if the process is the first tail proc
int Last_Proc_Finish;   // ID of the last process finished
static int quantum;      // Value of time for each process to run in the processor


// Create a process and add it to the RR queue
Process *proc_create(void (*function)(void*), void *arg)
{
    Process *context = malloc(sizeof(jmp_buf)); // Memory allocation for the context
    struct Process *proc = malloc(sizeof(Process)); // Memory allocation for the process
    context_create(context, function, (void *) arg); // Create the context of the process
    proc->context = context;    // Store the context in process struct
    proc->id = process_count;   // Asign the ID to the process
    proc->state = READY;        // Set the process ready to run

    if(head==NULL)  // if the list is empty add the process first
    {
        struct Queue *first = malloc(sizeof(struct Queue)); // Memory allocation of the first process
        first->proc = proc;     // Save the process as the current process
        first->nextproc = NULL; // There's no next process
        first->prevproc = NULL; // There's no previous process
        head = first;   // Set the head to first process
        tail = first;   // Also set the tail to first process
        free(first);    // Free the memory related to first
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
        free(newp);     // Free the memory used by newp
    }
    return proc;    // Return the process created
}

// Obtain the id of the process
int proc_id(void)
{
    return current->proc->id;
}

// Swith the control to the scheduler
void proc_yield(void)
{
    context_switch(running, &main);
}

// Remove the process from the queue
void proc_remove(void)
{
    Queue *temp = current->nextproc;    // Store the next process
    current->prevproc->nextproc = current->nextproc;    // Update the prev proc
    current->nextproc->prevproc = current->prevproc;    // Update the next proc
    free(current->proc->context);   // Free the memory used by the context
    free(current->proc);            // Free the rest of the process memory
    free(current);                  // Free the memory used by the queue element
    current = temp;                 // Set the next process as current
}

// Finish the process
void proc_finished(void)
{
    Last_Proc_Finish = current->proc->id;   // Save the id as last process finished
    current->proc->state = FINISHED;        // Set the status of the process as finished
    context_switch(running, &main);         // Return the control to the scheduler
}

// Scheduler
void proc_join(Process *proc)
{
    current = head;     // Set the process in the head as current
    time_t now;         // Current time in clock cycles

    while(TRUE)
    {
        if(Last_Proc_Finish == proc->id)
        {
            break; // Process process is finished so break
        }

        if(current->proc->state == FINISHED)
        {
            // Do nothing and pass to the next process
        }

        struct sigaction action;        // Struct to manage the signals
        action.sa_handler = proc_yield; // The signal handler is the process_yield function
        action.sa_flags = 0;            // NO flags needed
        sigfillset(&action.sa_mask);
        sigaction(SIGALRM, &action, NULL);

        struct itimerval timer;             // Create a timer from sys
        timer.it_value.tv_sec = quantum;    // Usable time by every process
        timer.it_interval = timer.it_value;
        setitimer(ITIMER_REAL, &timer, NULL);

        //********* add the lottery ticket method *********//

        //*************************************************//

        if(current->proc->state == READY)
        {
            running = current->proc->context;
            context_switch(&main, running);     // Run the current process
        }

        current = current->nextproc;    // When the prev proc end running start the next one
    }
}