
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <setjmp.h>
#include <assert.h>

#include "exceptions.h"

#define BASE_MASK   0xFFFFF0000000
#define PART_MASK   0x00000FFFFFFF

/*
 * The call stack is used when an error is generated by the system ir by an
 * unhandled exception.
 */
typedef struct __cstk__ {
    const char* file;
    const char* func;
    int line;
    struct __cstk__* next;
} call_stack, *call_stack_ptr;

/*
 * This holds the information for the original raise(), in case the exception
 * is unhandled, this facilitates displaying the error message.
 */
typedef struct {
    const char* fname;
    const char* hname;
    int line;
} func_id, *func_id_ptr;

/*
 * Static variables.
 *
 * This functionality is not thread safe.
 */
static jmp_buf_stack_ptr jmpstack = NULL;
static call_stack_ptr callstack = NULL;
static func_id funcid;
static uint64_t base_val;

/*
 * Dump the call stack to stderr in the event of an error.
 */
static void dump_call_stack() {

    //fprintf(stderr, "\nCall stack:\n");
    fprintf(stderr, "-----------------------------------\n");
    fprintf(stderr, "      file name:line   func name\n");
    fprintf(stderr, "-----------------------------------\n");
    call_stack_ptr tmp, next;
    for(tmp = callstack; tmp != NULL; tmp = next) {
        next = tmp->next;
        fprintf(stderr, "%15s:%-7d%s\n", tmp->file, tmp->line, tmp->func);
    }
    fprintf(stderr, "\n");
}

/*
 * This is the handler for an unhandled exception. This should not be
 * overridden or caused not to exit the program.
 */
static void unhandled_exception()
{
    fprintf(stderr, "\nUnhandled Exception: \"%s\" from %s:%d\n",
            funcid.hname, funcid.fname, funcid.line);
    dump_call_stack();
    exit(1);
}


/*
 * Destroy the call stack. Called by atexit().
 */
static void destroy_call_stack() {

    call_stack_ptr tmp, next;
    for(tmp = callstack; tmp != NULL; tmp = next) {
        next = tmp->next;
        free(tmp);
    }
}

/*
 * Free the memory for the jump stack.
 */
static void destroy_jmp_stack() {

    jmp_buf_stack_ptr tmp, next;
    for(tmp = jmpstack; tmp != NULL; tmp = next) {
        next = tmp->next;
        free(tmp);
    }
}

/*
 * Convert a function pointer to an integer.
 */
static int make_fint(exception_handler_func_ptr fname) {

    return (int)((uint64_t)fname & PART_MASK);
}

/*
 * Push an item on the call stack.
 */
void push_call_stack(const char* file, const char* func, int line) {

    call_stack_ptr ptr = malloc(sizeof(call_stack));
    assert(ptr != NULL);

    ptr->file = file;
    ptr->func = func;
    ptr->line = line;

    ptr->next = callstack;
    callstack = ptr;
}

/*
 * Pop an item from the call stack.
 */
void pop_call_stack() {

    call_stack_ptr tmp = callstack;
    callstack = tmp->next;

    free(tmp);
}

/*
 * Convert an integer back to a function pointer.
 */
exception_handler_func_ptr make_excep_hand_ptr(int val) {

    if(val == 0)
        return 0;
    else
        return (exception_handler_func_ptr)(base_val | val);
}

/*
 * Longjmp to the exception handler.
 */
void raise_exception(exception_handler_func_ptr fp,
                     const char* hname,
                     const char* fname,
                     int line) {

    if(fname != NULL) {
        funcid.fname = fname;
        funcid.hname = hname;
        funcid.line = line;
    }

    //fprintf(stderr, "raise: %p: %p\n", fp, js);
    longjmp(jmpstack->jbuf, make_fint(fp));
}

/*
 * Allocate an exception (not the handler) and push it on the stack. Returns
 * a pointer to the new handler.
 */
jmp_buf_stack_ptr push_exception() {

    jmp_buf_stack_ptr sp = malloc(sizeof(jmp_buf_stack));
    assert(sp != NULL);

    sp->next = jmpstack;
    jmpstack = sp;

    return sp;
}

/*
 * Pop an exception from the exception stack and free its memory.
 */
void pop_exception() {

    assert(jmpstack != NULL);
    jmp_buf_stack_ptr tmp = jmpstack;

    jmpstack = tmp->next;
    free(tmp);
}

/*
 * Main signal handler. This attempts to print a call stack whenever the
 * program was abroted from an external signal.
 */
void signal_handler(int sig) {

    switch(sig) {
        case SIGHUP:    fprintf(stderr, "SIGHUP received\n");   break;
        case SIGINT:    fprintf(stderr, "SIGINT received\n");   break;
        case SIGQUIT:   fprintf(stderr, "SIGQUIT received\n");  break;
        case SIGILL:    fprintf(stderr, "SIGILL received\n");   break;
        case SIGTRAP:   fprintf(stderr, "SIGTRAP received\n");  break;
        case SIGABRT:   fprintf(stderr, "SIGABRT received\n");  break;
        case SIGSEGV:   fprintf(stderr, "SIGSEGV received\n");  break;
        default: fprintf(stderr, "Unknown signal received\n");  break;
    }
    dump_call_stack();
    exit(sig); // with atexit() handling
}

/*
 * Initialize the data structures needed by the exception handlers. This also
 * catches various signals so that the call stack trace will be displayed if
 * there is an error during execution.
 */
void init_exceptions() {

    atexit(destroy_call_stack);
    atexit(destroy_jmp_stack);

    signal(SIGHUP, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGILL, signal_handler);
    signal(SIGTRAP, signal_handler);
    signal(SIGABRT, signal_handler);
    signal(SIGSEGV, signal_handler);

    base_val = (uint64_t)init_exceptions & BASE_MASK;

    jmp_buf_stack_ptr sp = malloc(sizeof(jmp_buf_stack));
    assert(sp != NULL);

    sp->next = jmpstack;
    jmpstack = sp;
    exception_handler_func_ptr f = make_excep_hand_ptr(setjmp(jmpstack->jbuf));

    if(f != 0) {
        if(jmpstack != NULL)
            unhandled_exception();
        else {
            fprintf(stderr, "internal error: unhandled exception\n");
            exit(1);
        }
    }
}

