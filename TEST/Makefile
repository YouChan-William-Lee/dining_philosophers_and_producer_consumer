CC= gcc
CFLAGS= -Wall -pedantic
LFLAGS= -lpthread

all: Dining Prodconsum

Dining: simulation.c
	$(CC) $(CFLAGS) $(LFLAGS) simulation.c -o Dining -D DINING

Prodconsum: simulation.c
	$(CC) $(CFLAGS) $(LFLAGS) simulation.c -o Prodconsum -D PRODCONSUM

clean:
	rm -f simulation *.o
