# Name: Ryan Brooks, Charlie Chen
# ID: 1530605, 1530488
# Course: CMPUT 275 winter 2019
# assignment: Final project: basic search engine
CC=g++
CFLAGS=-c -Wall -O2 -std=c++11
LFLAGS=
PROGS= search
OBJS= search.o

all: search.o
	@$(CC) search.o -o search $(LFLAGS)

search.o: search.cpp heap.h
	@$(CC) search.cpp -o search.o $(CFLAGS)

clean:
	@rm $(OBJS) $(PROGS)