CC= gcc
CFLAGS= -Wall -o
LFLAGS= -lpthread

all: simulation

a: simuation
	$(CC) $(LFLAGS) $(CFLAGS) Dining simulation.c -D Dining
	$(CC) $(LFLAGS) $(CFLAGS) Prodconsum simulation.c -D PRODCONSUM

clean:
	rm -f $(OBJECTS) *.o
