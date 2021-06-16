#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <setjmp.h>
#include "context.h"

#define TRUE        1
#define FALSE       0
#define RUNNING     0
#define READY       1
#define FINISHED  2

typedef struct Process
{
    int id;
    int state;
    int burst;
    float progress;
    double result;
    jmp_buf *context;
} Process;

typedef struct Queue
{
    struct Process *proc;       // Pointer to the current process
    struct Queue *nextproc;     // Pointer to the next process
    struct Queue *prevproc;     // Pointer to the previous process
} Queue;

Process *proc_create(void (*function)(void*), void *arg);
int proc_id(void);
void proc_yield(void);
void proc_remove(void);
void proc_finished(void);
void proc_join(Process *proc);
void proc_sleep(unsigned int time);

