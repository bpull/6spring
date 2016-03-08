# Makefile for SI 335 project 2

CPPFLAGS=-O3 -Wall -Wno-sign-compare -Wno-unused-function

rsa: rsa.o posint.o
	g++ $(CPPFLAGS) -o $@ $^

%.o: %.cpp posint.hpp
	g++ $(CPPFLAGS) -c $<

.PHONY: clean

clean:
	rm -f *.o rsa
