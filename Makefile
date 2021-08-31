CC= gcc
CFLAGS= -Wall -o
LFLAGS= -lpthread

all: a c

a: a.c
	$(CC) $(LFLAGS) $(CFLAGS) a a.c

c: c.c
	$(CC) $(LFLAGS) $(CFLAGS) c c.c

clean:
	rm -f $(OBJECTS) *.o
