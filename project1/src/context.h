#include <setjmp.h>

void context_create(jmp_buf *context, void (*function_addr)(void *), void *function_arg); 
int context_save(jmp_buf *context);
void context_restore(jmp_buf *context);
void context_switch(jmp_buf *old_context, jmp_buf *new_context);