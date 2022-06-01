CFLAGS      = -O3
LFLAGS      = -O3
CC      = gcc

OBJ     = heap.o maze.o dijkstra.o testall.o -lm
dstarlite:  $(OBJ)
	$(CC) $(LFLAGS) -o testall $(OBJ)

.cc.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm *.o
