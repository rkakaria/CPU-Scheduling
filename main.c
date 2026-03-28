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

algorithm_t schedule = RR;
#define TIME_QUANTUM 5

static const char* status_to_string(process_state status){
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

bool preemption_rr(job_t* current_job){
    return (current_job->time_quantum == 0); // we are preempting for the next tick
}

int main(){
    // int selection = input();
    job_t** jobs = parse(); // Array to store all jobs, sorted by arrival time.
    printf("Jobs parsed:\n");
    int num_jobs = number_jobs(); // Total number of jobs.
    int complete_jobs = 0; // Number of completed jobs.
    int pointer = 0; // Tracks which jobs in the array have arrived.
    int clock = 0; // Simulation clock.

    // Define Preemption Function
    bool (*preemption)(job_t*) = NULL;
    if (schedule == RR){
        preemption = preemption_rr;
    }else if (schedule == SJF){
    }else if (schedule == MLFQ){
    }
    
    os_srand(1); // Seed the random number generator.
    TRACE_LOG("[TRACE t=%d] scheduler_start num_jobs=%d\n", clock, num_jobs);
   
    // Initialize the ready queue with jobs that have arrived at t=0.
    queue_t* ready_q = create_queue();
    pointer = add_jobs(clock, pointer, num_jobs, jobs, ready_q);
    TRACE_LOG("[TRACE t=%d] initial_arrivals pointer=%d\n", clock, pointer);

    queue_t* io_q = create_queue(); // Queue for jobs waiting on I/O.
    job_t* current_job = NULL;
    

    while(complete_jobs < num_jobs){ 

        if (ready_q->elements > 0){
            current_job = dequeue(ready_q);
            current_job->status = RUNNING;
            current_job->time_quantum = TIME_QUANTUM;
            trace_job_state("Current Job set to: %d", clock, current_job);
        }

        while(true){ // Main process management loop.
            clock++;

            if (current_job != NULL){
                current_job->cpu_time++;
                current_job->remaining_time--;
                current_job->time_quantum--;
                trace_job_state("TICK", clock, current_job);
            }

            // Step 1: Add newly arriving jobs to the ready queue.
            pointer = add_jobs(clock, pointer, num_jobs, jobs, ready_q); 
            TRACE_LOG("[TRACE t=%d] add_jobs pointer=%d\n", clock, pointer);


            // Step 2: Move jobs that finished I/O from wait queue to ready queue.
            wake_io(io_q, ready_q);
            TRACE_LOG("[TRACE t=%d] wake_io_checked\n", clock);

            // If there is no active job, update bookkeeping and break.
            if (current_job == NULL || current_job->status == TERMINATED){
                update_queue_bookkeeping(io_q, ready_q, schedule);
                trace_job_state("break_null_or_terminated", clock, current_job);
                break;
             }

             if (current_job -> remaining_time == 0){
                // Step 3a: If the current job is complete, mark it terminated and update stats.
                // Update stats.
                current_job->total_time = clock - current_job->arrival_time;

                // Update state to terminated.
                current_job->status = TERMINATED;
                trace_job_state("job_terminated", clock, current_job);

                // Increment the number of completed jobs.
                complete_jobs++;
                TRACE_LOG("[TRACE t=%d] complete_jobs=%d/%d\n", clock, complete_jobs, num_jobs);

             } else if (preemption(current_job)){
                // Step 4: If the current job is preempted, move it back to ready.
                // Switch status back to ready -> this will trigger the break after the bookkeeping
                current_job->status = READY;
                // Add back to ready queue.
                enqueue(ready_q, current_job);
                trace_job_state("PREEMPTED", clock, current_job);
             } else{
                // Step 5: If the job requests I/O, move it to the I/O queue.
                if (IO_request()){
                    // Mark as waiting on I/O.
                    current_job->status = WAITING;
                    // Add to I/O queue.
                    enqueue(io_q, current_job);
                    trace_job_state("move current job to io queue due to IO request", clock, current_job);
                }
             }

            // Bookkeeping for all queued jobs this tick, then sort the ready queue.
            if (complete_jobs < num_jobs){
                update_queue_bookkeeping(io_q, ready_q, schedule);
            }

            if (current_job == NULL || current_job->status != RUNNING){
                trace_job_state("Current Job is not running, breaking loop", clock, current_job);
                break;
             }

        } // End main process management loop.
    } // End simulation loop when all jobs are completed.
   
    // Sort jobs by PID and display statistics.
    qsort(jobs, num_jobs, sizeof(jobs[0]), comp_pid);

    print_header();
    print_data(clock, jobs, num_jobs);

    // Free allocated memory.
    for(int i = 0; i < num_jobs; i++){
        free(jobs[i]);
        jobs[i] = NULL;
    }

    free(jobs);
    destroy_queue(ready_q);
    destroy_queue(io_q);
    
    return 0;
}