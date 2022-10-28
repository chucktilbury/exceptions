/*
 * Exceptions for the C language.
 *
 * This is a partial implementation of exceptions using non-local GOTOs for use
 * for error handling in C programs.
 *
 * Exceptions are raised and caught using an enum handle. This handle has to be
 * specified in the CATCH macro as well as when raising the exception.
 *
 * Exception BEGIN_TRY{} block jmp_buf's are stored in a stack. If there is a
 * CATCH{} block on the top of the stack, then it is popped and jongjmp()ed. If
 * there is a CATCH() {} clause for that handle, then that code is entered, else
 * the next one is tried. This happens until there are no more TRY{} blocks on
 * the stack and in that case, the exception is considered to be unhandled and
 * the program is exited with an error message.
 *
 * A call stack could be maintained with modifications to the user's source
 * code, but that is not acceptable for this simple solution.
 *
 * Most of the code associated wit this functionality -must- be in a macro for
 * it to work. This is because the setjmp() stores the -current- state of the
 * program. That means that there can be no function calls between that and
 * the mandatory if() that follows.
 *
 */
#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

typedef struct _ex_stack_ {
    jmp_buf buf;
    struct _ex_stack_* next;
} _ExStack;

extern _ExStack* exstack;

#define RAISE(V) do { \
        jmp_buf buf; \
        _ExStack* ptr = exstack; \
        memcpy(buf, ptr->buf, sizeof(jmp_buf)); \
        exstack = ptr->next; \
        free(ptr); \
        longjmp(buf, (V)); \
    } while(0)

#define BEGIN_TRY do{ \
    _ExStack* ptr = malloc(sizeof(_ExStack)); \
    if(ptr == NULL) { \
        fprintf(stderr, "memory allocation error\n"); \
        abort(); \
    } \
    ptr->next = exstack; \
    exstack = ptr; \
    int _exception_number = setjmp(ptr->buf); \
    if(_exception_number == 0)

#define CATCH(V) else if(_exception_number == (V))

#define END_TRY  else { \
    if(exstack != NULL) { \
        RAISE(_exception_number); \
    } \
    else { \
        fprintf(stderr, "Unhandled exception: %d\n", _exception_number); \
        abort(); \
    }}} while(0);


#endif
