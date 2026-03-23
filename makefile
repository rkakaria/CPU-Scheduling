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

io_list: io_list.o
	$(CC) io_list.o -o io_list

main: main.o sort.o queue.o scheduler.o io_list.o
	$(CC) main.o sort.o queue.o scheduler.o io_list.o -o main

scheduler.o: scheduler.c
	$(CC) -c $(CFLAGS) scheduler.c

queue.o: queue.c
	$(CC) -c $(CFLAGS) queue.c

sort.o: sort.c
	$(CC) -c $(CFLAGS) sort.c

io_list.o: io_list.c
	$(CC) -c $(CFLAGS) io_list.c

main.o: main.c
	$(CC) -c $(CFLAGS) main.c

.PHONY: clean
clean:
	rm -rf *.o $(TARGETS)

