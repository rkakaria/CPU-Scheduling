#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "queue.h"
#include "sort.h"

void os_srand(int seed);

int input();

int comp(const void* a, const void* b);

int comp_pid(const void* a, const void* b);

int number_jobs();

job_t** parse();

int IO_request();

int IO_complete();

int os_rand();

int add_jobs(int clock, int pointer, int num_jobs, job_t** jobs, queue_t* ready_q);

void wake_io(queue_t* io_q, queue_t* ready_q);

void update_queue_bookkeeping(queue_t* io_q, queue_t* ready_q, algorithm_t schedule);


#endif