#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "queue.h"
#include "io_list.h"
#include "sort.h"
#include "scheduler.h"


// Jonas: MLFS
// Reva: Preemptive SJF
// Nikhil: Round Robin


int main(){
#define TRACE(fmt, ...) printf("[TRACE t=%d] " fmt, clock, ##__VA_ARGS__)
    // int selection = input();

    job_t** jobs = parse(); // all jobs
    int num_jobs = number_jobs();
    int pointer = 0; // pointer to keep track of which jobs in sorted array have arrived

    /* initialize clock */
    int clock = 0;
    /* seeds the random number generator if you use rand(), or srand() */
    os_srand(1); // wrapper - call srand if you use stdnlib rand()
   
    // add new incoming jobs to the "ready to run" queue
    queue_t* ready_q = create_queue(); // ready jobs queue
   
    // enqueue jobs that arrived at t0
    TRACE("loaded %d jobs from input_file.txt\n", num_jobs);
    for(int i = 0; i < num_jobs; i++){
        if(jobs[i]->arrival_time > 0){
            pointer = i;
            break;
        }

        enqueue(ready_q, jobs[i]);
        TRACE("initial-arrival pid=%d enqueued_to_ready (ready=%d)\n",
              jobs[i]->pid, ready_q->elements);
    }

    io_list_t* io_q = io_list_create(); // jobs sleeping on io
    
    while(ready_q->elements > 0){ // main coordinator loop
        job_t* current_job = dequeue(ready_q);
        current_job->status = RUNNING;
        TRACE("dispatch pid=%d from_ready (ready=%d io=%d rem=%d)\n",
              current_job->pid, ready_q->elements, io_q->elements, current_job->remaining_time);

        while(true){ /* this loop is the main process management loop */

            /**
             * Step 1: Add new arriving jobs to the ready to run queue
             */
            TRACE("Step 1\n");
            // add new incoming jobs to the ready to run state
            for(int i = pointer; i < num_jobs; i++){
                // check if the job is already in the ready to run queue
                if (jobs[i]->status == READY){
                    continue;
                }
                // check if job has arrived
                if(jobs[i]->arrival_time > clock){
                    pointer = i;
                    break;
                }

                // update state to ready
                jobs[i]->status = READY;
                enqueue(ready_q, jobs[i]);
                TRACE("arrival pid=%d enqueued_to_ready (ready=%d)\n",
                      jobs[i]->pid, ready_q->elements);
            }

            /**
             * Step 2: Check to see if any of the jobs in the wait queue have completed their I/O -> if yes add to ready to run queue
             */
             TRACE("Step 2\n");
            io_node_t* current = io_list_begin(io_q);
            while (current != io_list_end(io_q)) {
                io_node_t* next = current->next;
                current->data->io_time++; // increment io time for each job in the io 
                TRACE("io-check pid=%d io_time=%d\n",
                      current->data->pid, current->data->io_time);

                // check to see if the job has completed its I/O
                if (IO_complete()) {
                    int completed_pid = current->data->pid;
                    // remove from io queue and add to the ready to run queue
                    enqueue(ready_q, current->data);
                    io_list_remove_node(io_q, current);
                    TRACE("io-complete pid=%d moved_to_ready (ready=%d io=%d)\n",
                          completed_pid, ready_q->elements, io_q->elements);
                }
                current = next;
            }
        

            /**
             * Step 3a: Check to see if the current job has finished its work -> if yes mark as swapped out (job complete, exit)
             * update stats and bookkeeping for the job, increment clock, break from loop
             */
             TRACE("Step 3a\n");
             if (current_job -> remaining_time == 1){
                // update stats
                current_job->total_time = clock - current_job->arrival_time;
                current_job->cpu_time = current_job->total_time;

                // update state to terminated
                current_job->status = TERMINATED;
                // increment clock
                clock++;
                TRACE("terminate pid=%d total=%d cpu=%d (ready=%d io=%d)\n",
                      current_job->pid, current_job->total_time, current_job->cpu_time,
                      ready_q->elements, io_q->elements);
                // break from loop
                break;
             }


            /**
             * Step 4: Check to see if the current job has been preempted -> if yes mark as swapped out (preempted)
             * update state: 
             */
            // if you run a preemptive scheduler and there are now higher
            // priority jobs on the "ready to run" state then :
                // mark current_job as swapped out (preempted)


            /**
             * Step 5: Check for I/O request from current job -> if yes mark as swapped out (sleeping on I/O)
             */
            if (IO_request()){
                // mark as swapped out (sleeping on I/O)
                current_job->status = WAITING;
                // add to io queue
                io_list_append(io_q, current_job);
                TRACE("io-request pid=%d moved_to_io (io=%d)\n",
                      current_job->pid, io_q->elements);
            }


            // if status == 1 /* need to do I/O */
                // then mark current_job as swapped out (sleeping on I/O)
            // else if this job has been on the CPU for an entire time-slice
                // then mark the job as swapped out (end of time-slice) and
                // move it to the ready to run state


            // do bookkeeping and statistics
            clock++;
            // if current_ job was swapped out then move the job to the appropriate
            // queue and break from this loop

            break;
        } /* End while main process management loop */
        
        TRACE("end-outer-iteration ready=%d io=%d\n", ready_q->elements, io_q->elements);
    } /* End while if there are no jobs in the system */
   
    TRACE("simulation-end\n");
    #undef TRACE
    free_jobs(jobs);
    destroy_queue(ready_q);
    io_list_destroy(io_q);
    
    return 0;
}