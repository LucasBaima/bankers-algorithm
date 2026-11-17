
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

OBJ = main.o banker.o

all: banker

banker: $(OBJ)
	$(CC) $(CFLAGS) -o banker $(OBJ)

main.o: main.c banker.h
	$(CC) $(CFLAGS) -c main.c

banker.o: banker.c banker.h
	$(CC) $(CFLAGS) -c banker.c

clean:
	rm -f $(OBJ) banker

.PHONY: all clean
