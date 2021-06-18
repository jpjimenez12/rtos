#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>

#define TRUE 1
#define FALSE 0

#define STACK 65536 //Stack size of 64 kB

void context_create(jmp_buf *context, void (*function_addr)(void *), void *function_arg); 
int context_save(jmp_buf *context);
void context_restore(jmp_buf *context);
void context_switch(jmp_buf *old_context, jmp_buf *new_context);
void unblock_signals(void);
void aux_funct(void);