#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>


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

void context_create(jmp_buf *context, void (*function_addr)(void *), void *function_arg)
{
    
}