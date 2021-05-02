CC=g++
CFLAGS=-c -Wall -O2 -std=c++11
LFLAGS=
PROGS= digraph_test reachability breadthfirstsearch depthfirstsearch
OBJS= digraph.o digraph_test.o reachability.o breadthfirstsearch.o depthfirstsearch.o

# executable targets
all: digraph_test reachability breadthfirstsearch depthfirstsearch

digraph_test: digraph_test.o digraph.o
	$(CC) digraph_test.o digraph.o -o digraph_test $(LFLAGS)

reachability: reachability.o digraph.o
	$(CC) reachability.o digraph.o -o reachability $(LFLAGS)

breadthfirstsearch: breadthfirstsearch.o digraph.o
	$(CC) breadthfirstsearch.o digraph.o -o breadthfirstsearch $(LFLAGS)

depthfirstsearch: depthfirstsearch.o digraph.o
	$(CC) depthfirstsearch.o digraph.o -o depthfirstsearch $(LFLAGS)

# object targets
digraph_test.o: digraph_test.cpp digraph.h
	$(CC) digraph_test.cpp -o digraph_test.o $(CFLAGS)

digraph.o: digraph.cpp digraph.h
	$(CC) digraph.cpp -o digraph.o $(CFLAGS)

reachability.o: reachability.cpp digraph.h
	$(CC) reachability.cpp -o reachability.o $(CFLAGS)

breadthfirstsearch.o: breadthfirstsearch.cpp digraph.h
	$(CC) breadthfirstsearch.cpp -o breadthfirstsearch.o $(CFLAGS)

depthfirstsearch.o: depthfirstsearch.cpp digraph.h
	$(CC) depthfirstsearch.cpp -o depthfirstsearch.o $(CFLAGS)

clean:
	@rm $(OBJS) $(PROGS)
