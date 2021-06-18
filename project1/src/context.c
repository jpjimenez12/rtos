#include "context.h"


static jmp_buf  caller;                     // Context that call the create function
static jmp_buf  *new;                       // New context to be created
static jmp_buf  original_context;
static sigset_t new_signals;
static void     (*new_function)(void *);
static void     *new_args;


int context_save(jmp_buf *context)
{
    return sigset(context, 1);      // Save the execution context
}

void context_restore(jmp_buf *context)
{
    return siglongjmp(context, 1);  // Restore the execution context
}

void context_switch(jmp_buf *old_context, jmp_buf *new_context)
{
    if (context_save(old_context) == 0) // Ensure current context was saved
    {
        context_restore(new_context);   // Restore new context
    }    
}

void unblock_signals()
{
    sigset_t sigset;
    sigset_t original_sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGALRM);
    sigaddset(&sigset, SIGVTALRM); 
    sigprocmask(SIG_UNBLOCK, &sigset, &original_sigset);
}

// This aux function starts when USR1 is activated
void aux_funct()
{
    if(context_save(new) == FALSE)
    {
        return;
    }

    // After the context is saved it proceed to load a clean stack
    void (* function)(void *);  // Pointer to start running the function
    void *args;                 // Pointer to pass arguments to the function

    sigprocmask(SIG_SETMASK, &new_signals, NULL);

    function = new_function;
    args = new_args;

    context_switch(new, &caller);

    unblock_signals();

    function(args);     // Run the function

    fprintf(stderr, "There is a problem, maybe process exit problem.\n");
    abort();
}

void context_create(jmp_buf *context, void (*function_addr)(void *), void *function_arg)
{
    sigset_t original_signal;       
    sigset_t signal;        // Create a signal set to manage the context
    sigemptyset(&signal);           // Empty the signal set
    sigaddset(&signal, SIGUSR1);    
    sigprocmask(SIG_BLOCK, &signal, &original_signal);

    // 
    struct sigaction original_action;
    struct sigaction action;
    memset((void *)&action, 0, sizeof(struct sigaction));
    action.sa_handler = aux_funct;
    action.sa_flags = SA_ONSTACK;
    sigemptyset(&action.sa_mask);
    sigaction(SIGUSR1, &action, &original_action);

    // Create a new stack to save the new context
    void *stack_addr = malloc(STACK);   // Allocate in memory the stack
    stack_t alt_stack;
    stack_t original_alt_stack;
    alt_stack.ss_sp = stack_addr;
    alt_stack.ss_size = STACK;
    alt_stack.ss_flags = 0;
    sigaltstack(&alt_stack, &original_alt_stack);

    // save all the variables needed
    new = context;
    new_function = function_addr;
    new_args = function_arg;
    new_signals = original_signal;

    raise(SIGUSR1); // Start the handler using the alternative stack
    sigfillset(&signal);
    sigdelset(&signal, SIGUSR1);
    sigsuspend(&signal);

    sigaltstack(NULL, &alt_stack);
    alt_stack.ss_flags = SS_DISABLE;
    sigaltstack(&alt_stack, NULL);
    if(!(original_alt_stack.ss_flags & SS_DISABLE))
    {
        sigaltstack(&original_alt_stack, NULL);
    }
    sigaction(SIGUSR1, &original_signal, NULL);
    sigprocmask(SIG_SETMASK, &original_signal, NULL);

    //  Switch to the new context
    context_switch(&caller, context);

    return;
}