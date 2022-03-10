

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "exceptions.h"

/*
 * Define the exception handlers
 */
void handler1() {
    fprintf(stderr, "in the handler1\n");
}

void handler2() {
    fprintf(stderr, "in the handler2\n");
}

void handler3() {
    fprintf(stderr, "in the handler3\n");
}

/*
 * Funcitons to call
 */
void func4() {
    ENTER;

    fprintf(stderr, "func4 before raise\n");
    RAISE(handler3);
    fprintf(stderr, "func4 after raise\n");

    RETURN;
}

void func3() {
    ENTER;

    fprintf(stderr, "func3 before raise\n");
    RAISE(handler2);
    fprintf(stderr, "func3 after raise\n");

    RETURN;
}

void func2() {
    ENTER;

    fprintf(stderr, "func2 before raise\n");

// different ways to create an error for a stack dump
//     RAISE(handler3);
//
//     *(int*)0 = 1;
//
//     void* p = malloc(1);
//     free(p);
//     free(p);
//
//     fgetc(stdin);

    fprintf(stderr, "func2 after raise\n");

    RETURN;
}

void func1() {
    ENTER;

    fprintf(stderr, "here is func1\n");

    TRY {
        func2();
        TRY {
            func3();
        }
        EXCEPT(handler3) {
            fprintf(stderr, "handler3: func1\n");
        }
        END_EXCEPT
    }
    EXCEPT(handler2) {
        fprintf(stderr, "handler2: func1\n");
        TRY {
            func4();
        }
        EXCEPT(handler3) {
            fprintf(stderr, "caught 3 from handler 2\n");
        }
        END_EXCEPT
    }
    END_EXCEPT

    RETURN;
}

/*
 * Main()
 */
int main() {

    init_exceptions();
    ENTER;

    fprintf(stderr, "handler1: %p\n", handler1);
    fprintf(stderr, "handler2: %p\n", handler2);
    fprintf(stderr, "handler3: %p\n\n", handler3);

    TRY {
        func1();
    }
    EXCEPT(handler1) {
        fprintf(stderr, "handler1: main\n");
    }
    EXCEPT(handler2) {
        fprintf(stderr, "handler2: main\n");
    }
    END_EXCEPT

    fprintf(stderr, "finishing up\n\n");

    RETURN_VAL(0);
}
