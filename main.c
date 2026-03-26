#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h> // remove
#include "queue.h"
#include "sort.h"
#include "scheduler.h"
#include "display.h"

int main(){

    algorithm_t algorithm = input();

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

    queue_t* io_q = create_queue(); // queue for jobs sleeping on io

    /* Main coordinator loop. Runs as long as there are jobs
    that have not yet completed. */
    while(complete_jobs < num_jobs){ 

        ready_q->head = merge_sort(ready_q->head, SJF); // sort ready queue by time remaining for sjf
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

            // check for jobs completing I/O
            wake_io(io_q, ready_q);

            // re-sort after adding jobs done with I/O
            ready_q->head = merge_sort(ready_q->head, algorithm);

            if(current_job == NULL){ // check that there's a running job
                break;
            }
            
            // current_job->status = RUNNING;
            current_job->remaining_time--;
            current_job->cpu_time++;

            if(current_job->remaining_time <= 0){ // if job has finished
                current_job->status = TERMINATED; 
                complete_jobs++;
            } else if(ready_q->elements > 0){ // if there are other ready jobs
                job_t* shortest = ready_q->head->data;

                if((shortest->remaining_time < current_job->remaining_time) || (shortest->remaining_time == current_job->remaining_time && shortest->pid < current_job->pid)){ // if there is a higher priority job
                    current_job->status = READY; // mark current_job as swapped out (preempted)
                    enqueue(ready_q, current_job);
                }
            } 
            
            if(IO_request() == 1 && current_job->status == RUNNING){ // check for I/O request from running current_job
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