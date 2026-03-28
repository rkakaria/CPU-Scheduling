#include <stdlib.h>
#include <stdbool.h>
#include "queue.h"
#include "sort.h"
#include "scheduler.h"
#include "display.h"
#include <stdio.h>

#ifndef TRACE
#define TRACE 1
#endif

#if TRACE
#define TRACE_LOG(...) do { fprintf(stderr, __VA_ARGS__); } while (0)
#else
#define TRACE_LOG(...) do { } while (0)
#endif

#define TIME_QUANTUM 5

static const char* status_to_string(state_t status){
    switch(status){
        case NEW: return "NEW";
        case READY: return "READY";
        case RUNNING: return "RUNNING";
        case WAITING: return "WAITING";
        case TERMINATED: return "TERMINATED";
        default: return "UNKNOWN";
    }
}

static void trace_job_state(const char* event, int clock, job_t* job){
    if(job == NULL){
        TRACE_LOG("[TRACE t=%d] %s job=NULL\n", clock, event);
        return;
    }

    TRACE_LOG(
        "[TRACE t=%d] %s pid=%d status=%s remaining=%d cpu=%d arrival=%d\n",
        clock,
        event,
        job->pid,
        status_to_string(job->status),
        job->remaining_time,
        job->cpu_time,
        job->arrival_time
    );
}

int main(){

    algorithm_t algorithm = input(); //

    job_t** jobs = parse(); // array to store all jobs, sorted by arrival time
    int num_jobs = number_jobs(); // number of total jobs
    int complete_jobs = 0; // number of completed jobs
    int pointer = 0; // track which jobs in the jobs array have arrived
    int clock = 0; // initialize clock
    
    os_srand(1); // seed the random number generator
   
    /* Initialize ready queue with jobs that have arrived
    at t0 */
    queue_t* ready_q = create_queue();
    pointer = add_jobs(clock, pointer, num_jobs, jobs, ready_q);
    ready_q->head = merge_sort(ready_q->head, algorithm); // sort for the given algorithm

    queue_t* io_q = create_queue(); // queue for jobs sleeping on io

    job_t* current_job = NULL;

    /* Main coordinator loop. Runs as long as there are jobs
    that have not yet completed. */
    while(complete_jobs < num_jobs){ 

        if(ready_q->elements > 0){
            current_job = dequeue(ready_q);
            current_job->status = RUNNING;
            current_job->time_quantum = TIME_QUANTUM;
            trace_job_state("Current Job set to", clock, current_job);
        }

        while(true){ // Main process management loop.
            clock++;

            if (current_job != NULL){
                current_job->cpu_time++;
                current_job->remaining_time--;
                current_job->time_quantum--;
                trace_job_state("TICK", clock, current_job);
            }

            // Bookkeeping for all queued jobs this tick, then sort the ready queue.
            update_queue_bookkeeping(io_q, ready_q);

            // Step 1: Add newly arriving jobs to the ready queue.
            pointer = add_jobs(clock, pointer, num_jobs, jobs, ready_q); 
            TRACE_LOG("[TRACE t=%d] add_jobs pointer=%d\n", clock, pointer);

            // check for jobs completing I/O
            wake_io(io_q, ready_q);

            // re-sort after adding new jobs and those done with I/O
            if(ready_q->head != NULL && ready_q->head->next != NULL && algorithm != RR){
                ready_q->head = merge_sort(ready_q->head, algorithm);
            }

            if(current_job == NULL){ // check that there's a running job
                break;
            }

            if(current_job->remaining_time == 0){ // if job has finished
                current_job->status = TERMINATED; 
                current_job->total_time = clock - current_job->arrival_time;
                complete_jobs++;
                current_job = NULL; // reset current job
                TRACE_LOG("[TRACE t=%d] complete_jobs=%d/%d\n", clock, complete_jobs, num_jobs);

             } else if (preemption(algorithm, ready_q, current_job) == true){
                // Step 4: If the current job is preempted, move it back to ready.
                // Switch status back to ready -> this will trigger the break after the bookkeeping
                current_job->status = READY;
                enqueue(ready_q, current_job);
                trace_job_state("PREEMPTED", clock, current_job);
                current_job = NULL;

             } else{
                // Step 5: If the job requests I/O, move it to the I/O queue.
                if (IO_request()){
                    // Mark as waiting on I/O.
                    current_job->status = WAITING;
                    // Add to I/O queue.
                    enqueue(io_q, current_job);
                    current_job = NULL;
                    trace_job_state("move current job to io queue due to IO request", clock, current_job);
                }
             }

            if (current_job == NULL || current_job->status != RUNNING){
                trace_job_state("Current Job is not running, breaking loop", clock, current_job);
                break;
            }
        }
    }

    // sort jobs by pid and display statistics
    qsort(jobs, num_jobs, sizeof(jobs[0]), comp_pid);

    print_header();
    print_data(clock, jobs, num_jobs);
   
    // free memory
    for(int i = 0; i < num_jobs; i++){
        free(jobs[i]);
        jobs[i] = NULL;
    }

    free(jobs);
    destroy_queue(ready_q);
    destroy_queue(io_q);
    
    return 0;
}