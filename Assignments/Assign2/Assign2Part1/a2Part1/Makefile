CC = g++
CFLAGS = -c -Wall -O2 -std=c++11
LFLAGS =
OBJS= Server.o dijkstra.o digraph.o
PROGS = server
server: Server.o dijkstra.o digraph.o
	$(CC) Server.o -o dijkstra.o -o digraph.o -o server $(CFLAGS)
dijkstra.o: dijkstra.cpp dijkstra.h heap.h digraph.h
	$(CC) dijkstra.cpp -o dijkstra.o $(CLAGS)
Server.o: Server.cpp wdigraph.h dijkstra.h digraph.h
	$(CC) Server.cpp -o Server.o $(CLAGS)
digraph.o: digraph.cpp digraph.h
	$(CC) digraph.cpp -o digraph.o $(CLAGS)
clean:
	rm -f $(PROGS)
	rm -f $(OBJS)