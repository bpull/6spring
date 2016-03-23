CC = gcc
CFLAGS = -g -O2
OMPFLAGS = -fopenmp
LIBS = -lm


all: ex1 ex2 ex3 ex4

ex1: ex1.c
	$(CC) $(CFLAGS) -o $@ ex1.c $(OMPFLAGS) $(LIBS)

ex2: ex2.c
	$(CC) $(CFLAGS) -o $@ ex2.c $(OMPFLAGS) $(LIBS)

ex3: ex3.c
	$(CC) $(CFLAGS) -o $@ ex3.c $(OMPFLAGS) $(LIBS)

ex4: ex4.c
	$(CC) $(CFLAGS) -o $@ ex4.c $(OMPFLAGS) $(LIBS)

clean:
	rm -fr ex1 ex2 ex3 ex4
