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

algorithm_t schedule = SJF;

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

int main(){

    algorithm_t algorithm = input();

    job_t** jobs = parse(); // array to store all jobs, sorted by arrival time
    int num_jobs = number_jobs(); // number of total jobs
    int complete_jobs = 0; // number of completed jobs
    int pointer = 0; // track which jobs in the jobs array have arrived
    int clock = 0; // initialize clock
    
    os_srand(1); // Seed the random number generator.
    TRACE_LOG("[TRACE t=%d] scheduler_start num_jobs=%d\n", clock, num_jobs);
   
    // Initialize the ready queue with jobs that have arrived at t=0.
    queue_t* ready_q = create_queue();
    pointer = add_jobs(clock, pointer, num_jobs, jobs, ready_q);
    ready_q->head = merge_sort(ready_q->head, algorithm); // sort for the given algorithm

    queue_t* io_q = create_queue(); // queue for jobs sleeping on io

    /* Main coordinator loop. Runs as long as there are jobs
    that have not yet completed. */
    while(complete_jobs < num_jobs){ 
        job_t* current_job = dequeue(ready_q);

        if(current_job != NULL){
            current_job->status = RUNNING;
        }
        
        while(true){ // main process management loop 
            /* Clock increments at beginning of iteration 
            so that jobs arriving at t = clock are added to the 
            ready queue while the current job is running such that
            they can preempt at the end of the iteration if
            necessary. */
            clock++;          

            /* Bookkeeping and statistics:
            Update total time for all incomplete jobs.
            Update ready time for all ready jobs.
            I/O time is updated via the wake_io function. Updates times based
            on the status each job has at the beginning of the iteration. */ 
            for(int i = 0; i < num_jobs; i++){
                if(jobs[i]->status != TERMINATED && jobs[i]->status != NEW){
                    jobs[i]->total_time++;
                }
                if(jobs[i]->status == READY){
                    jobs[i]->ready_time++;
                }
            }

            /* Add new incoming jobs to the ready queue, which will start
            being processed during the next iteration */ 
            pointer = add_jobs(clock, pointer, num_jobs, jobs, ready_q);   


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

             } else if (preemption){
                // Step 4: If the current job is preempted, move it back to ready.
                // Switch status back to ready -> this will trigger the break after the bookkeeping
                current_job->status = READY;
                // Add back to ready queue.
                enqueue(ready_q, current_job);
                trace_job_state("job_preempted", clock, current_job);
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

            // re-sort after adding new jobs and those done with I/O
            ready_q->head = merge_sort(ready_q->head, algorithm);

            if(current_job == NULL){ // check that there's a running job
                break;
            }
            
            current_job->remaining_time--;
            current_job->cpu_time++;

            if(current_job->remaining_time <= 0){ // if job has finished
                current_job->status = TERMINATED; 
                complete_jobs++;
            } else if(preemption(algorithm, ready_q, current_job)){ // if job is preempted
                current_job->status = READY;
                enqueue(ready_q, current_job);
            } else if(IO_request() == 1){ // check for I/O request from running current_job
                current_job->status = WAITING; 
                enqueue(io_q, current_job);
            } 
            
            // if current job has been swapped out
            if(current_job->status != RUNNING){
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