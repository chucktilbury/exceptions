

all: tst

tst:
	gcc -Wall -Wextra -g -o tst test.c

clean:
	-rm -f tst
