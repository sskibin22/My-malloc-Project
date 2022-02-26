CC = gcc
CFLAGS = -std=c99 -Wall

test: memgrind.o basic_tests.o mymalloc.o
	$(CC) $(CFLAGS) $^ -o $@

memgrind.o: mymalloc.h basic_tests.h
basic_tests.o: mymalloc.h
mymalloc.o: mymalloc.h