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
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include <time.h>
#include <string.h>

#define STACK_SIZE 32000
#define NUM_OF_QUEUES 5
#define QUANTUM 10000
//thread status
#define READY 1
#define YIELDED 2
#define RUNNING 3
#define WAITING 4
#define FINISHED 5
#define REMOVE 6
#define BLOCKED 7
//lock status
#define UNLOCKED 0
#define LOCKED 1

typedef uint mypthread_t;

	/* add important states in a thread control block */
typedef struct threadControlBlock
{
	mypthread_t id;
	mypthread_t waitingThread; //thread that it is waiting to finish
	int status; //Thread status
	ucontext_t context;	//priorty for scheduler to choose which thread to select next
	
	int elapsedQuantums; //number of quantums that thread already run
	void** valPtr; //original arg
	void* returnVal; //returned at termination of thread


} tcb;

// Feel free to add your own auxiliary data structures (linked list or queue etc...)
//linked list used for queue
typedef struct threadQueue{
	
	tcb* TCB;
	struct threadQueue* next;
	struct threadQueue* prev;

} queue;

/* mutex struct definition */
typedef struct mypthread_mutex_t
{
	// YOUR CODE HERE
	int lock;
	queue* blockedQueue;
} mypthread_mutex_t;

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

void timer_init(struct sigaction timer, struct itimerval interval);
static void schedule();
static void sched_RR();
static void sched_PSJF();
void enqueue(tcb* thread, queue** argQ);
tcb* dequeue(queue** argQ);
tcb* getTCB(queue** argQ, mypthread_t thread);
void readyThreads(queue** argQ, mypthread_t thread);
void freeTCBQueue(void);
void freeThread(queue* argQ);
int isFinished(queue** argQ, mypthread_t thread);
int removeThread(queue** argQ, mypthread_t thread);

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
