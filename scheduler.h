#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "queue.h"

void os_srand(int seed);

int input();

int comp(const void* a, const void* b);

int number_jobs();

job_t** parse();

void free_jobs(job_t** jobs);

int IO_request();

int IO_complete();

int os_rand();

#endif