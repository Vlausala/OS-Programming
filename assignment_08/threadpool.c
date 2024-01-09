/*
threadpool.c
Threadpool implementation for Assignment 8

Author: Valtteri Lausala
Course: CS-C3140 - Operating Systems, Lecture, 5.9.2023-8.12.2023
Date: 16.11.2023

*/

#include "threadpool.h"
#include <stdio.h>
#include <stdlib.h>


/**
 * Initializes a thread pool, by setting required variables, allocating memory to queues, and initializing mutexes & semaphore
 *
 * @param pool Pointer to the ThreadPool
 * @param num_threads Number of created threads
 * @param job_size Size of the jobs queue
 */
void thread_pool_init(ThreadPool *pool, int num_threads, int job_size) {

    //Step1: Initialize variables and queues
    pool->jobs = (Job*) malloc( sizeof(Job) * job_size );
    pool->threads = (Thread*) malloc( sizeof(Thread) * num_threads );
    pool->job_count = 0;
    pool->job_size = job_size;
    pool->front = 0;
    pool->num_threads = num_threads;
    pool->stop_requested = 0;

    //Step2: Initialize mutexes and semaphores
    int isSuccess = 0;
    isSuccess = pthread_mutex_init(&pool->lock, NULL);
    if (isSuccess != 0) {free(pool->jobs); free(pool->threads); exit(-1); }

    isSuccess = pthread_mutex_init(&pool->job_lock, NULL);
    if (isSuccess != 0) {free(pool->jobs); free(pool->threads); exit(-1); }

    isSuccess = sem_init(&pool->jobs_available, 0, 0);
    if (isSuccess != 0) {free(pool->jobs); free(pool->threads); exit(-1); }

    //Step3: Create threads and allocate and run worker thread
    for (int i=0; i<num_threads; i++){
        WorkerInput* workerInput = (WorkerInput*) malloc( sizeof(WorkerInput) );

        workerInput->pool = pool;
        workerInput->thread = &pool->threads[i];
        pool->threads[i].id = i;

        pthread_create(&pool->threads[i].thread, NULL, worker_thread, (void*) workerInput );
    }
}


/**
 * Submits a job to the thread pool. If the job queue is full, job is discarded
 *
 * @param pool Pointer to the ThreadPool
 * @param job Job to be added
 */
void thread_pool_submit(ThreadPool *pool, Job job) {

    //Step1: Lock job queue mutex
    pthread_mutex_lock(&pool->lock);

    //Step2: Check if queue full and free args if it is
    if (pool->job_count == pool->job_size){
        printf("job queue is full!\n");

        if (job.should_free && !job.is_freed) {
            free(job.args);
            job.is_freed = 1;
        }
        pthread_mutex_unlock(&pool->lock);
        return;
    }

    //Step3: Add job to correct position and increment job count
    pool->jobs[pool->job_count] = job;
    pool->job_count++;

    //Step4: Signal job available
    sem_post(&pool->jobs_available);

    //Step5: Unlock lock from queue
    pthread_mutex_unlock(&pool->lock);
}


/**
 * Executes queued jobs. When threadpool needs to stop, cleans up after the threads
 *
 * @param args Pointer to a WorkerInput structure containing threadpool and thread pointers
 * @return NULL after the thread finishes its execution.
 */
void* worker_thread(void* args) {
    //Step1: Access information from WorkerInput
    WorkerInput* input = (WorkerInput*) args;
    ThreadPool* pool = input->pool;
    Thread* thread = input->thread;

    //Step2: Run while loop for running the thread
    while(1){

        //Step3: Wait for jobs and stop the thread if needed
        if(pool->stop_requested){ break; }
        sem_wait(&pool->jobs_available);
        if(pool->stop_requested){ break; }

        //Step4: Lock job queue and dequeue Job FIFO style
        pthread_mutex_lock(&pool->lock);
        if (pool->front == pool->job_count){
            pthread_mutex_unlock(&pool->lock);
            continue;
        }
        Job job = pool->jobs[pool->front];
        pool->front = (pool->front + 1);
        pthread_mutex_unlock(&pool->lock);

        //Step5: Lock job mutex if current job needs it
        if (job.run_safely){
            pthread_mutex_lock(&pool->job_lock);
            if (!job.is_freed){
                job.function(job.args);
            }
            pthread_mutex_unlock(&pool->job_lock);
        } else {
            if (!job.is_freed){
                job.function(job.args);
            }
        }
        //Step6: Free args if needed
        if (job.should_free && !job.is_freed) {
            free(job.args);
            job.is_freed = 1;
        }

    }
    //Step7: After threads stop, clean up and return
    printf("thread with id %d is finished.\n", thread->id);
    free(input);
    return NULL;
}


/**
 * Signals the thread pool to stop
 *
 * @param pool Pointer to the ThreadPool
 */
void thread_pool_stop(ThreadPool *pool) {
    //Step1: Set flag to stop the threadpool
    pool->stop_requested = 1;

    //Step2: loop through the threads and signal, so no job gets stuck
    for (int i = 0; i < pool->num_threads; i++) {
        sem_post(&pool->jobs_available);
    }
}


/**
 * Waits for all worker threads to finish
 *
 * @param pool Pointer to the ThreadPool
 */
void thread_pool_wait(ThreadPool *pool) {
    //Step1: Wait all threads to finish
    for (int i=0; i<pool->num_threads; i++) {
        pthread_join(pool->threads[i].thread, NULL);
    }
}


/**
 * Cleans up resources after threadpool is stopped
 *
 * @param pool Pointer to the ThreadPool
 */
void thread_pool_clean(ThreadPool *pool) {

    //Step1: Clean all args for jobs, if there are any left
    pthread_mutex_lock(&pool->lock);
    if (pool->front < pool-> job_count){
        for (int i=pool->front; i < pool->job_count; i++) {
            Job *job = &pool->jobs[i];

            if (job->should_free && !job->is_freed) {
                free(job->args);
                job->is_freed = 1;
            }
        }
    }
    pthread_mutex_unlock(&pool->lock);


    //Step2: Destroy mutexes and semaphores
    pthread_mutex_destroy(&pool->lock);
    pthread_mutex_destroy(&pool->job_lock);
    sem_destroy(&pool->jobs_available);

    //Step3: Free the arrays
    free(pool->jobs);
    free(pool->threads);
}
//EOF