

all: etest libexceptions.a

libexceptions.a:
	gcc -Wall -Wextra -g -c -o exceptions.o exceptions.c
	ar -cr libexceptions.a exceptions.o

etest: libexceptions.a
	gcc -Wall -Wextra -g -o etest etest.c -L. -lexceptions

clean:
	-rm -f etest libexceptions.a *.o
