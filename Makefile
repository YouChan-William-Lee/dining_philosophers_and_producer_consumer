Target= a
CC= gcc
CFLAGS= -Wall -o
LFLAGS= -lpthread

all: $(Target)

$(Target): $(Target).c
	$(CC) $(LFLAGS) $(CFLAGS) $(Target) $(Target).c

clean:
	rm -f $(Target) *.o
