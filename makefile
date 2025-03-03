CC = gcc
CFLAGS = -Wall -Wextra

all: rods

rods: main.o rods.o
	$(CC) $(CFLAGS) -o rods main.o rods.o

main.o: main.c rods.h
	$(CC) $(CFLAGS) -c main.c

rods.o: rods.c rods.h
	$(CC) $(CFLAGS) -c rods.c

clean:
	rm -f rods *.o