#ifndef DISPLAY_H
#define DISPLAY_H

#include "queue.h"

void print_header();

void print_data(int clock, job_t** jobs, int num_jobs);

void print_footer();

#endif