

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdint.h>
#include <setjmp.h>

/*
 * Type definition for an exception handler.
 */
typedef void (*exception_handler_func_ptr)();

/*
 * The stack of jmp_buf's facilitates nesting and handling the exceptions in a
 * location other than where it was generated.
 */
typedef struct __sptr__ {
    jmp_buf jbuf;
    struct __sptr__* next;
} jmp_buf_stack, *jmp_buf_stack_ptr;

/*
 * Macros for the actual exception implementation. See the example for an
 * example.
 */
#define TRY { \
        jmp_buf_stack_ptr sp = push_exception(); \
        exception_handler_func_ptr f = make_excep_hand_ptr(setjmp(sp->jbuf)); \
        if(f == 0) {

#define EXCEPT(handler_name) } else if(f == handler_name) { \
            pop_exception(); \
            handler_name();

#define END_EXCEPT } else { \
            pop_exception(); \
            raise_exception(f, NULL, NULL, 0); \
        }}

#define RAISE(fp) raise_exception(fp, #fp, __func__, __LINE__)

/*
 * These macros are for the call stack. The call stack is required if you want
 * it to display if there is an unhandled exception. The call stack is also
 * displayed if a signal is caught.
 */
#define ENTER push_call_stack(__FILE__, __func__, __LINE__)
#define RETURN do { pop_call_stack(); return; } while(0)
#define RETURN_VAL(v) do { pop_call_stack(); return(v); } while(0)

/*
 * None of these functions should ever be accessed directly. Macros are
 * provided for all functionality.
 */
void push_call_stack(const char*, const char*, int);
void pop_call_stack();

exception_handler_func_ptr make_excep_hand_ptr(int);
void raise_exception(exception_handler_func_ptr, const char*, const char*, int);
jmp_buf_stack_ptr push_exception();
void pop_exception();
void init_exceptions();


#endif
