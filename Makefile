CC= gcc
CFLAGS= -Wall -pedantic
LFLAGS= -lpthread

all: Dining_Philosophers Producer_Consumer

Dining_Philosophers: simulation.c
	$(CC) $(CFLAGS) $(LFLAGS) simulation.c -o Dining_Philosophers -D DINING_PHILOSOPHERS

Producer_Consumer: simulation.c
	$(CC) $(CFLAGS) $(LFLAGS) simulation.c -o Producer_Consumer -D PRODUCER_CONSUMER

clean:
	rm -f Dining_Philosophers Producer_Consumer *.o
