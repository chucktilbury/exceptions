/*
 * This is a simple test for the exceptions.
 *
 * Build string:
 * clang -Wall -Wextra -g -o tst test.c
 *
 * Mess around with different combinations of exec path to
 * verify that the exception behave in an expected manner.
 *
 */
#include <stdio.h>
#include <signal.h>
#include "exceptions.h"

_ExStack* exstack = NULL;

enum {
    EXCEPTION_ONE = 20,
    EXCEPTION_TWO = 50,
    DUMMY_EXCEPT,
    EXIT_EXCEPT = 8080,
};

void (*old_exit)(int) = exit;
#define exit(v) RAISE(EXIT_EXCEPT)

void abort_handler(int sig) {

    (void)sig;
    fprintf(stderr, "in the abort handler\n");
}

void func1();
void func2();
void func3(int x);
void another();

void func1() {

    printf("func1()\n");
    func2();
}

void func2() {

    printf("func2()\n");
    BEGIN_TRY {
        func3(0);
        // This is here to prove that nested exception handlers
        // can compile and work.
        BEGIN_TRY {
            // this cannot be entered.
            printf("in the nested begin try\n");
            func3(1);
        }
        CATCH(EXCEPTION_TWO) {
            printf("int the nested exception handler\n");
        }
        END_TRY
        another(2);
    }
    CATCH(EXCEPTION_TWO) {
        printf("caught exception 50\n"); // probably error
    }
    END_TRY
}

void func3(int x) {

    printf("func3(%d) before\n", x);
    if(x == 0)
        RAISE(EXCEPTION_ONE);
    else if(x == 1)
        RAISE(EXCEPTION_TWO);

    printf("func3() after\n");
}

void another(int n) {

    printf("in another: %d\n", n); // should not happen
    exit(1);
}

int main() {

    signal(SIGABRT, abort_handler);

    BEGIN_TRY {
        printf("in the top try\n");
        func1();
        another(1);
    }
    CATCH(EXCEPTION_ONE) {
        printf("caught exception 20\n");
    }
    END_TRY

    printf("function main() is about to return\n");

    return 0;
}
