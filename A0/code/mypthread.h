// File:	mypthread_t.h

// List all group members' names: Lorenzo Ladao
// iLab machine tested on: ilab2

#ifndef MYTHREAD_T_H
#define MYTHREAD_T_H

#define _GNU_SOURCE

/* in order to use the built-in Linux pthread library as a control for benchmarking, you have to comment the USE_MYTHREAD macro */
#define USE_MYTHREAD 1

/* include lib header files that you need here: */
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

//thread status
#define READY 1
#define YIELD 2
#define BLOCKED 3
#define RUNNING 4

typedef uint mypthread_t;

	/* add important states in a thread control block */
typedef struct threadControlBlock
{
	mypthread_t id;
	//Thread status
	int status;
	//priorty for scheduler to choose which thread to select next
	int priority;
	//points to threads that were created by this thread
	tcb* next;
	//points to thread that created this thread
	tcb* address;

} tcb;

typedef struct queue{

	tcb* head;
	tcb* tail;

} queue;

typedef struct scheduler{

	queue* Queue;
	tcb* currentThread;
	//if the thread is blocked, or yielded then switch thread
	int change;

} scheduler;

/* mutex struct definition */
typedef struct mypthread_mutex_t
{

	// YOUR CODE HERE
	
} mypthread_mutex_t;


// Feel free to add your own auxiliary data structures (linked list or queue etc...)



/* Function Declarations: */

/* create a new thread */
int mypthread_create(mypthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg);

/* current thread voluntarily surrenders its remaining runtime for other threads to use */
int mypthread_yield();

/* terminate a thread */
void mypthread_exit(void *value_ptr);

/* wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr);

/* initialize a mutex */
int mypthread_mutex_init(mypthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);

/* aquire a mutex (lock) */
int mypthread_mutex_lock(mypthread_mutex_t *mutex);

/* release a mutex (unlock) */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex);

/* destroy a mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex);

#ifdef USE_MYTHREAD
#define pthread_t mypthread_t
#define pthread_mutex_t mypthread_mutex_t
#define pthread_create mypthread_create
#define pthread_exit mypthread_exit
#define pthread_join mypthread_join
#define pthread_mutex_init mypthread_mutex_init
#define pthread_mutex_lock mypthread_mutex_lock
#define pthread_mutex_unlock mypthread_mutex_unlock
#define pthread_mutex_destroy mypthread_mutex_destroy
#endif

#endif