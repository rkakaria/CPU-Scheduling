#include <stdlib.h>
#include <stdbool.h>
#include "queue.h"
#include "sort.h"
#include "scheduler.h"
#include "display.h"

int main(){
    // int selection = input();

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
    while(complete_jobs < num_jobs){ // main coordinator loop
        
        job_t* current_job = dequeue(ready_q);

        while(true){ /* this loop is the main process management loop */

            /* Clock increments at beginning of iteration 
            so that jobs arriving at t = clock are added to the 
            ready queue while the current job is running such that
            they can preempt at the end of the iteration if
            necessary. */
            clock++;

            // bookkeeping and statistics

            // add new incoming jobs
            pointer = add_jobs(clock, pointer, num_jobs, jobs, ready_q); 

            // check for jobs completing I/O
            wake_io(io_q, ready_q);

            // re-sort ready queue after adding jobs done with I/O

            // conditionals to check if the job is complete, pre-empted, or going to I/O

            break;
        } /* End while main process management loop */
        
        break;
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