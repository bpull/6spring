
firehalo: firehalo.o firesendrecv.o
	mpicc -o firehalo firehalo.o firesendrecv.o -lm

firehalo.o: firehalo.c
	mpicc -c firehalo.c

firesendrecv.o: firesendrecv.c
	mpicc -c firesendrecv.c

clean:
	rm -f *.o firehalo
