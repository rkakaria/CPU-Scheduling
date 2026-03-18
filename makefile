CC := gcc

TARGETS := main 

CFLAGS := -Wall

all: $(TARGETS)

scheduler: scheduler.o
	$(CC) scheduler.o -o scheduler

sort: sort.o
	$(CC) sort.o -o sort

queue: queue.o
	$(CC) queue.o -o queue

main: main.o sort.o queue.o scheduler.o 
	$(CC) main.o sort.o queue.o scheduler.o -o main

scheduler.o: scheduler.c
	$(CC) -c $(CFLAGS) scheduler.c

queue.o: queue.c
	$(CC) -c $(CFLAGS) queue.c

sort.o: sort.c
	$(CC) -c $(CFLAGS) sort.c

main.o: main.c
	$(CC) -c $(CFLAGS) main.c

.PHONY: clean
clean:
	rm -rf *.o $(TARGETS)

