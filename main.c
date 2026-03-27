#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "queue.h"
#include "sort.h"
#include "scheduler.h"
#include "display.h"
#include "sort.h"
#include <stdio.h>

#ifndef TRACE
#define TRACE 1
#endif

#if TRACE
#define TRACE_LOG(...) do { fprintf(stderr, __VA_ARGS__); } while (0)
#else
#define TRACE_LOG(...) do { } while (0)
#endif

algorithm_t schedule = MLFQ;

#define MLFQ_MAX_PRIORITY 2
#define MLFQ_BOOST_INTERVAL 25

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
    // int selection = input();

    job_t** jobs = parse(); // array to store all jobs, sorted by arrival time
    int num_jobs = number_jobs(); // number of total jobs
    int complete_jobs = 0; // number of completed jobs
    int pointer = 0; // track which jobs in the jobs array have arrived
    int clock = 0; // initialize clock
    
    os_srand(1); // seed the random number generator
    TRACE_LOG("[TRACE t=%d] scheduler_start num_jobs=%d\n", clock, num_jobs);
   
    /* Initialize ready queue with jobs that have arrived
    at t0 */
    queue_t* ready_q = create_queue();
    pointer = add_jobs(clock, pointer, num_jobs, jobs, ready_q);
    TRACE_LOG("[TRACE t=%d] initial_arrivals pointer=%d\n", clock, pointer);

    queue_t* io_q = create_queue(); // queue for jobs sleeping on io
    job_t* current_job = NULL;
    

    while(complete_jobs < num_jobs){ // main coordinator loop

        if (ready_q->elements > 0){
            current_job = dequeue(ready_q);
            current_job->status = RUNNING;
            current_job->time_slice_used = 0;
            trace_job_state("Current Job set to: %d", clock, current_job);
        }

        while(true){ /* this loop is the main process management loop */
            clock++;            
            bool preemption = false; // placeholder for preemption
            
            if (schedule == MLFQ && current_job != NULL) {
                if (current_job->time_slice_used >= 2){
                    if (current_job->priority < MLFQ_MAX_PRIORITY) {
                        current_job->priority++;
                    }
                    current_job -> time_slice_used = 0;
                    preemption = true;
                } else {
                    current_job -> time_slice_used++;
                }
            }

            if (current_job != NULL){
                current_job->cpu_time++;
                current_job->remaining_time--;
                trace_job_state("tick", clock, current_job);
            }

            /**
             * Step 1: Add new arriving jobs to the ready to run queue
             */
            // add new incoming jobs
            pointer = add_jobs(clock, pointer, num_jobs, jobs, ready_q); 
            TRACE_LOG("[TRACE t=%d] add_jobs pointer=%d\n", clock, pointer);


            /**
             * Step 2: Check to see if any of the jobs in the wait queue have completed their I/O -> if yes add to ready to run queue
             */
            // check for jobs completing I/O
            wake_io(io_q, ready_q);
            TRACE_LOG("[TRACE t=%d] wake_io_checked\n", clock);

            // check if the current job is null if so bookkeep and break
            if (current_job == NULL || current_job->status == TERMINATED){
                update_queue_bookkeeping(io_q, ready_q, schedule);
                trace_job_state("break_null_or_terminated", clock, current_job);
                break;
             }
        

            /**
             * Step 3a: Check to see if the current job has finished its work -> if yes mark as swapped out (job complete, exit)
             * update stats and bookkeeping for the job, increment clock, break from loop
             */
             if (current_job -> remaining_time == 0){
                // update stats
                current_job->total_time = clock - current_job->arrival_time;

                // update state to terminated
                current_job->status = TERMINATED;
                trace_job_state("job_terminated", clock, current_job);

                complete_jobs++;
                TRACE_LOG("[TRACE t=%d] complete_jobs=%d/%d\n", clock, complete_jobs, num_jobs);

             } else if (preemption){
                
                /**
                * Step 4: Check to see if the current job has been preempted -> if yes mark as swapped out (preempted)
                * update state: 
                */
                // switch status back to ready
                current_job->status = READY;
                // add back to ready queue
                enqueue(ready_q, current_job);
                trace_job_state("job_preempted", clock, current_job);
             } else{
                // IO Request
                /**
                * Step 5: Check for I/O request from current job -> if yes mark as swapped out (sleeping on I/O)
                */
                if (IO_request()){
                    // mark as swapped out (sleeping on I/O)
                    current_job->status = WAITING;
                    // add to io queue
                    enqueue(io_q, current_job);
                    trace_job_state("move current job to io queue due to IO request", clock, current_job);
                }
             }
            
            if (schedule == MLFQ){
                if (clock % MLFQ_BOOST_INTERVAL == MLFQ_BOOST_INTERVAL - 1){
                    for (int i = 0; i < num_jobs; i++) {
                        if (jobs[i]->status != TERMINATED) {
                            jobs[i]->priority = 0;
                            jobs[i]->time_slice_used = 0;
                        }
                    }
                }
            }

            // bookkeeping for all queued jobs for this tick and sort the ready queue
            update_queue_bookkeeping(io_q, ready_q, schedule);


            if (current_job == NULL || current_job->status != RUNNING){
                trace_job_state("Current Job is not running, breaking loop", clock, current_job);
                break;
             }

        } /* End while main process management loop */        


    } /* End while if there are no jobs in the system */
   
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