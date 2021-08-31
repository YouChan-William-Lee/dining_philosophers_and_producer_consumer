objects= a c
CC= gcc
CFLAGS= -Wall -o
LFLAGS= -lpthread

all: $(objects)

$(Target): $(Target).c
	$(CC) $(LFLAGS) $(CFLAGS) $(Target) $(Target).c

clean:
	rm -f $(Target) *.o
