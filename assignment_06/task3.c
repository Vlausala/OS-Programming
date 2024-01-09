#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

//Author: Valtteri Lausala
//Date: 25.10.2023
//Course: CS-C3140 - Operating Systems, Lecture, 5.9.2023-8.12.2023


// You are provided with the required structs for doing this assignment, you don't have to change these.
typedef struct {
    char job_id[50];
    int sleep_time;
    pthread_t thread;
    bool finished;
    bool started;
} Job;

typedef struct {
    int size;
    int count;
    Job** jobs;
} JobHolding;

// This function is used for running the job, you don't have to change this function.
// Note that the input for this function is void* not Job*.
void* runJob(void* arg) {
    Job* job = (Job*)arg;
    sleep(job->sleep_time);
    job->finished = true;
    printf("Job with id %s finishd with sleep time %d\n", job->job_id, job->sleep_time);
    return NULL;
}

// This function is used for printing a job. You don't have to change this function.
void printJob(Job* job) {
    printf("Job-> JobID: %s, Started: %d, Finished: %d, SleepTime: %d\n", job->job_id, job->started, job->finished, job->sleep_time);
}

Job* allocateJob(const char* job_id, int sleep_time) {

    Job* job = NULL;

    // Instructions: 
    // 1. You have to use calloc to allocate memory for the job
    // 2. You can use strcpy to initialize the job_id for job
    // 3. You have to set the sleep_time, finished, and started variables

    // YOUR CODE STARTS HERE

    //Allocate memory for Job-struct and cast the the pointer correctly
    job = (Job*) calloc( 1, sizeof(Job) );

    //Copy job id
    strcpy(job->job_id, job_id);

    //Set defaults
    job->sleep_time = sleep_time;
    job->finished = false;
    job->started = false; 

    // YOUR CODE ENDS HERE

    printf("Job created-> ID: %s, finished: %d, started: %d, sleep_time: %d\n", job->job_id, job->finished, job->started, job->sleep_time);
    return job;
}

void startJob(Job* job) {

    bool didStart = false;

    // Instructions:
    // 1. Check if the passed job is not already started
    // 2. If the job was not started, you have to create a thread that will call the runJob function and pass the job to it
    // 3. Set the started value for job as true.
    // 4. If the job and new thread was started, mark didStart as true for logging purposes.

    // YOUR CODE STARTS HERE
    if (job->started == false){
        //Create thread and assign the identifier for the job
        int isSuccessful = pthread_create( &(job->thread) , NULL, runJob, job);

        //If successful, pthread returns 0
        if (isSuccessful == 0) {
            job->started = true; 
            didStart = true;
        }
    }
    // YOUR CODE ENDS HERE

    if (didStart) {
        printf("Job started-> ID: %s, started: %d\n", job->job_id, job->started);
    } else {
        printf("Job %s was already started before!\n", job->job_id);
    }
}

void waitForJob(Job* job) {

    bool didWait = false;

    // Instructions:
    // 1. Check if the job has been started and is not finished. (mark didWait as true if you ended up waiting for the job)
    // 2. If so, wait for the thread associated with the job to finish using pthread_join.
    // 3. Once the thread has finished, set the job's 'finished' flag to true.

    // YOUR CODE STARTS HERE
    if (job->started && !job->finished){
        int isSuccessful = pthread_join( job->thread , NULL);

        //If successful, pthread returns 0
        if (isSuccessful == 0) {
            job->finished = true;
            didWait = true;
        }
    }



    // YOUR CODE ENDS HERE

    if (!didWait) {
        printf("Job %s has not been started yet or it's already finished.\n", job->job_id);
    } else {
        printf("Waited for job %s and it's now finished.\n", job->job_id);
    }
}

void initJobHolding(JobHolding* jobHolding, int initialSize) {
    // Instructions:
    // 1. Allocate memory for the 'jobs' array using calloc.
    // 2. Initialize 'size' and 'count' to their respective values.
    // 3. Initialize each element in the 'jobs' array to NULL.

    // YOUR CODE STARTS HERE
    jobHolding->jobs = (Job**) calloc(initialSize,sizeof(Job*));

    jobHolding->size = initialSize;
    jobHolding->count = 0;

    //Lets initialize the array by looping over it and setting elements to null
    for ( int i=0; i < initialSize; i++ ){
        jobHolding->jobs[i] = NULL;
    }
    // YOUR CODE ENDS HERE

    printf("Job holding structure initialized with size: %d\n", initialSize);
}


void cleanJobs(JobHolding* jobHolding, JobHolding* finishedJobs) {
    // Instructions:
    // 1. Iterate over the list of jobs in 'jobHolding' and detect the ones which are finished.
    // 2. Add the finished job to the 'finishedJobs' structure, and set the corresponding job in 'jobHolding' to NULL.
    // 3. Reduce the 'count' of 'jobHolding' and increase the 'count' of 'finishedJobs'.
    // 4. Create a new job array for 'jobHolding'. In this array, ensure there are no NULL jobs between defined jobs.
    // 5. Reduce the 'size' of the 'jobHolding' array by 50% if it's at most 25% full.
    // 6. Repeat step 4 and 5 as needed to minimize the size of the 'jobHolding' array.
    // 7. If during the cleaning all of the jobs are finished, you can set the initial value as size which is 2.
    
    // YOUR CODE STARTS HERE

    //Get amount of jobs
    int jobsAmount = jobHolding->count;

    //Initialize array for unfinished jobs
    Job** unfinishedJobs = calloc(jobHolding->size, sizeof(Job*));

    //Initialize size variable
    int newSize = jobHolding->size;

    //Initialize index for unfinished jobs- array
    int unfinishedArray = 0; 

    //Loop over the jobHolding array of jobs
    for(int i = 0; i < jobsAmount; i++) {

        //If the job is finished, add it to finished jobs and adjust counts
        if(jobHolding->jobs[i]->finished) {

            finishedJobs->jobs[finishedJobs->count] = jobHolding->jobs[i];            
            jobHolding->count--;
            finishedJobs->count++;

            //Calculate count to size ratio for jobHolding, if condition met, halve the size
            float ratio = ((float) jobHolding->count) / ((float) newSize);
            if(ratio <= 0.25f) { newSize = newSize / 2; }

        //If job is not finished, add it to the unfinished jobs array
        } else {
            
            unfinishedJobs[unfinishedArray] = jobHolding->jobs[i];
            unfinishedArray++;
        }
        // Overwrite the job with null
        jobHolding->jobs[i] = NULL;
    }

    // set 2 as the new size if all jobs are finished
    if(jobHolding->count == 0) { newSize = 2; }

    //if size is smaller, adjust the jobs array and update size
    if(newSize < jobHolding->size) 
    {
        // Old pointer may be invalidated
        jobHolding->jobs = realloc(jobHolding->jobs, newSize * sizeof(Job*));
        jobHolding->size = newSize;
    }

    // copy the unfinished jobs to the new array to ensure no NULL jobs between defined jobs
    for(int i = 0; i < jobHolding->count; i++) {
        jobHolding->jobs[i] = unfinishedJobs[i];
    }

    // Free unfinished jobs memory allocation
    free(unfinishedJobs);
    // YOUR CODE ENDS HERE

    printf("JobHolding is cleaned and the new size is: %d and the new count is: %d\n", jobHolding->size, jobHolding->count);

}

void extendJobs(JobHolding* jobHolding) {
    // Instructions:
    // 1. Calculate the new size of the jobs array.
    // 2. Create new empty array with the new size using calloc
    // 3. Copy the exicting jobs in the jobs array to the new array
    // 4. free the old array
    // 5. point the jobHolding->jobs to the new array

    // YOUR CODE STARTS HERE

    //Calculate double size
    int newSize = (jobHolding->size) * 2;

    //Allocate memory for the new size
    Job** jobArray = (Job**) calloc(newSize, sizeof(Job*) );

    //Move jobs to new bigger array
    for (int i=0; i < jobHolding->size; i++ ){
        jobArray[i] = jobHolding->jobs[i];
    }

    //Free old jobs array
    free(jobHolding->jobs);

    //Adjust array and size
    jobHolding->jobs = jobArray;
    jobHolding->size = newSize;

    // YOUR CODE ENDS HERE
    printf("JobHolding is extended to the size %d\n", jobHolding->size);
}

void createJob(JobHolding* jobHolding, char* job_id, int sleep_time) {
    // Instructions:
    // 1. Check if the 'jobHolding' structure is full, i.e., if 'count' is equal to 'size'.
    // 2. If it is full, call the 'extendJobs' function to increase the size of the 'jobHolding' structure.
    // 3. Allocate memory for a new job using the 'allocateJob' function with the provided 'job_id' and 'sleep_time'.
    // 4. Add the newly created job to the 'jobHolding' structure at the position specified by 'count'.
    // 5. Increment the 'count' of 'jobHolding' to account for the new job.

    // YOUR CODE STARTS HERE

    //Check if full, and extend if needed
    if ( jobHolding->count == jobHolding->size ) {
        extendJobs(jobHolding);
    }

    //Allocate memory for new job
    Job* newJob = allocateJob(job_id, sleep_time);

    //Add new job to jobs
    jobHolding->jobs[jobHolding->count] = newJob;

    //Update count 
    jobHolding->count++;


    // YOUR CODE ENDS HERE
}

void waitAll(JobHolding* jobHolding) {
    // Instructions:
    // 1. Iterate over the list of jobs in 'jobHolding'.
    // 2. For each non-NULL job that has been started, call the 'waitForJob' function to wait for its completion.

    // YOUR CODE STARTS HERE
    for (int i = 0; i < jobHolding->size; i++){
        Job* job = jobHolding->jobs[i];
        if (job != NULL && job->started){
            waitForJob(job);
        }
    }
    // YOUR CODE ENDS HERE
}

void startAll(JobHolding* jobHolding) {
    // Instructions:
    // 1. Iterate over the list of jobs in 'jobHolding'.
    // 2. For each non-NULL job that has not been started, call the 'startJob' function to start it.

    // YOUR CODE STARTS HERE
    for (int i = 0; i < jobHolding->size; i++){
        Job* job = jobHolding->jobs[i];
        if (job != NULL && !job->started){
            startJob(job);
        }
    }
    // YOUR CODE ENDS HERE
}

void findWaitJob(JobHolding* jobHolding, char* job_id) {
    // Instructions:
    // 1. Iterate over the list of jobs in 'jobHolding'.
    // 2. For each non-NULL job, check if its 'job_id' matches the provided 'job_id' using `strcmp`.
    // 3. If a matching job is found, call the 'waitForJob' function to wait for its completion.
    // 4. After waiting for the job, break out of the loop to ensure only one matching job is waited for.

    // YOUR CODE STARTS HERE
    for (int i = 0; i < jobHolding->size; i++){
        Job* job = jobHolding->jobs[i];
        if (job != NULL){
            int comparisonResult = strcmp(job->job_id, job_id);
            if (comparisonResult == 0){
                waitForJob(job);
                break;
            }
        }
    }
    // YOUR CODE ENDS HERE
}

void findStartJob(JobHolding* jobHolding, char* job_id) {
    // Instructions:
    // 1. Iterate over the list of jobs in 'jobHolding'.
    // 2. For each non-NULL job, check if its 'job_id' matches the provided 'job_id' using `strcmp`.
    // 3. If a matching job is found, call the 'startJob' function to start it.
    // 4. After starting the job, break out of the loop to ensure only one matching job is started.

    // YOUR CODE STARTS HERE
    for (int i = 0; i < jobHolding->size; i++){
        Job* job = jobHolding->jobs[i];
        if (job != NULL){
            int comparisonResult = strcmp(job->job_id, job_id);
            if (comparisonResult == 0){
                startJob(job);
                break;
            }
        }
    }
    // YOUR CODE ENDS HERE
}

void freeJobs(JobHolding* jobHolding) {
    // Instructions:
    // 1. Iterate over the list of jobs in 'jobHolding'.
    // 2. For each non-NULL job, use the `free` function to deallocate the memory associated with the job.
    // 3. After deallocating the memory for individual jobs, free the memory for the 'jobs' array itself.

    // YOUR CODE STARTS HERE
    for (int i = 0; i< jobHolding->size; i++){
        Job* job = jobHolding->jobs[i];
        if(job != NULL) { free(job); }
    }
    free(jobHolding->jobs);

    jobHolding->size = 0;
    jobHolding->count = 0;
    // YOUR CODE ENDS HERE
}

// This function will print out the jobs and status for all of the available 
void statJobs(JobHolding* jobHolding, JobHolding* finishedJobs) {
    printf("Stats for jobHolding -> Size of the job array: %d, Number of jobs: %d\n", jobHolding->size, jobHolding->count);
    for (int i = 0; i < jobHolding-> size; i++) {
        if (jobHolding-> jobs[i] != NULL) {
            printJob(jobHolding->jobs[i]);
        }
    }
    printf("Stats for finishedJobs -> Size of the job array: %d, Number of jobs: %d\n", finishedJobs->size, finishedJobs->count);
    for (int i = 0; i < finishedJobs->size; i++) {
        if (finishedJobs->jobs[i] != NULL) {
            printJob(finishedJobs->jobs[i]);
        }
    }
}

// Function to parse and execute commands
void executeCommand(JobHolding* jobHolding, JobHolding* finishedJobs, const char* command) {
    char job_id[50];
    int sleep_time;

    if (sscanf(command, "create job %s %d", job_id, &sleep_time) == 2) {
        createJob(jobHolding, job_id, sleep_time);
    } else if (sscanf(command, "start job %s", job_id) == 1) {
        findStartJob(jobHolding, job_id);
    } else if (sscanf(command, "wait job %s", job_id) == 1) {
        findWaitJob(jobHolding, job_id);
    } else if (strcmp(command, "start all") == 0) {
        startAll(jobHolding);
    } else if (strcmp(command, "wait all") == 0) {
        waitAll(jobHolding);
    } else if (strcmp(command, "clean") == 0) {
        cleanJobs(jobHolding, finishedJobs);
    } else if (strcmp(command, "extend") == 0) {
        extendJobs(jobHolding);
    } else if (strcmp(command, "stats") == 0) {
        statJobs(jobHolding, finishedJobs);
    } else if (sscanf(command, "sleep %d", &sleep_time) == 1) {
        printf("Sleeping for %d seconds!\n", sleep_time);
        sleep(sleep_time);
    } else {
        printf("Invalid command: %s\n", command);
    }
}

int main() {
    JobHolding jobHolding;
    JobHolding finishedJobs;
    initJobHolding(&jobHolding, 2);
    initJobHolding(&finishedJobs, 1000);

    while (1) {
        char command[100];
        fgets(command, sizeof(command), stdin);
        command[strlen(command) - 1] = 0;
        if (strcmp(command, "exit") == 0) {
            break;
        }
        executeCommand(&jobHolding, &finishedJobs, command);
    }
    freeJobs(&jobHolding);
    freeJobs(&finishedJobs);

    return 0;
}