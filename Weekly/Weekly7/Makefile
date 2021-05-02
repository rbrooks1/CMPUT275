# Name: Ryan Brooks
# ID: 1530605
# Course: CMPUT 275 winter 2019
# assignment: Weekly exercise 7: AVL trees
CC=g++
CFLAGS=-c -Wall -O2 -std=c++11
LFLAGS=
PROGS= ex7test
OBJS= ex7test.o

all: ex7test.o
	@$(CC) ex7test.o -o ex7test $(LFLAGS)

ex7test.o: ex7test.cpp avlmap.h avlaux.h
	@$(CC) ex7test.cpp -o ex7test.o $(CFLAGS)

clean:
	@rm $(OBJS) $(PROGS)