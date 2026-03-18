#include <stdlib.h>
#include <stdbool.h>
#include "queue.h"
#include "sort.h"
#include "scheduler.h"

int main(){
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
    for(int i = 0; i < num_jobs; i++){
        if(jobs[i]->arrival_time > 0){
            pointer = i;
            break;
        }

        enqueue(ready_q, jobs[i]);
    }

    queue_t* io_q = create_queue(); // queue for jobs sleeping on io
    
    while(ready_q->elements > 0){ // main coordinator loop
        job_t* current_job = dequeue(ready_q);

        while(true){ /* this loop is the main process management loop */

            // add new incoming jobs to the ready to run state
            for(int i = pointer; i < num_jobs; i++){
                if(jobs[i]->arrival_time > clock){
                    pointer = i;
                    break;
                }

                enqueue(ready_q, jobs[i]);
            }
            
            // while there are jobs waiting for I/O
                // status = IO_complete()
                // if status == 1
                // then put the job whose I/O has completed on the ready to run queue
            // if this job has finished its work (if time remaining is 1)
            // then mark current_job as swapped out (job complete, exit)
            // if you run a preemptive scheduler and there are now higher
            // priority jobs on the "ready to run" state then :
                // mark current_job as swapped out (preempted)
                /* check for I/O request from current_job */
                // if the job is not complete
                // then status = IO_request()
            // if status == 1 /* need to do I/O */
                // then mark current_job as swapped out (sleeping on I/O)
            // else if this job has been on the CPU for an entire time-slice
                // then mark the job as swapped out (end of time-slice) and
                // move it to the ready to run state
            // do bookkeeping and statistics
            // clock ++

            // if current_ job was swapped out then move the job to the appropriate
            // queue and break from this loop

            break;
        } /* End while main process management loop */
        
        dequeue(ready_q);
    } /* End while if there are no jobs in the system */
   
    free_jobs(jobs);
    destroy_queue(ready_q);
    destroy_queue(io_q);
    
    return 0;
}