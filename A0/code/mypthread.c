// File:	mypthread.c

// List all group members' names:
// iLab machine tested on:

#include "mypthread.h"

// INITAILIZE ALL YOUR VARIABLES HERE
// YOUR CODE HERE

//used to define the type of scheduler to use (0 = FIFO, 1 = RR, 2 = SJF)
#define SCHED 0


int threadID = 1;
int firstThread = 1;

/* create a new thread */
int mypthread_create(mypthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg)
{
	
	// create a Thread Control Block
	tcb* thisTCB = malloc(sizeof(*thisTCB));
	thisTCB->id = thread;
	thisTCB->next = NULL;
	thisTCB->priority = 0;
	thisTCB->status = READY;
	thisTCB->address = thread;

	//initialize context
	thisTCB->context.uc_link = NULL;
	//allocate heap space for the thread's stack
	thisTCB->context.uc_stack.ss_sp = malloc(STACK_SIZE);
	thisTCB->context.uc_stack.ss_size = STACK_SIZE;
	thisTCB->context.uc_stack.ss_flags = 0;

	// after everything is all set, push this thread into the ready queue

	

	return 0;
};

/* current thread voluntarily surrenders its remaining runtime for other threads to use */
int mypthread_yield()
{
	// YOUR CODE HERE
	
	// change current thread's state from Running to Ready
	// save context of this thread to its thread control block
	// switch from this thread's context to the scheduler's context

	return 0;
};

/* terminate a thread */
void mypthread_exit(void *value_ptr)
{
	// YOUR CODE HERE

	// preserve the return value pointer if not NULL
	// deallocate any dynamic memory allocated when starting this thread
	
	return;
};


/* Wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr)
{
	// YOUR CODE HERE

	// wait for a specific thread to terminate
	// deallocate any dynamic memory created by the joining thread

	return 0;
};

//inserts thread into queue
void enqueue(tcb* thread, queue* q){
	//queue is empty
	if(q->tail == NULL){
		q->head = thread;
		q->tail = thread;
	}
	//queue is not empty
	else{
		q->tail->next = thread;
		q->tail = thread;
	}
}

//returns NULL if queue is empty
tcb* dequeue(queue* q){
	//queue is empty
	if(q->head == NULL){
		return NULL;
	}
	//queue is not empty
	tcb* temp = q->head;
	q->head = q->head->next;
	temp->next = NULL;

	if(q->head == NULL){
		q->tail = NULL;
	}

	return temp;
}

/* initialize the mutex lock */
int mypthread_mutex_init(mypthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr)
{
	// YOUR CODE HERE
	
	//initialize data structures for this mutex

	return 0;
};

/* aquire a mutex lock */
int mypthread_mutex_lock(mypthread_mutex_t *mutex)
{
		// YOUR CODE HERE
	
		// use the built-in test-and-set atomic function to test the mutex
		// if the mutex is acquired successfully, return
		// if acquiring mutex fails, put the current thread on the blocked/waiting list and context switch to the scheduler thread
		
		return 0;
};

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex)
{
	// YOUR CODE HERE	
	
	// update the mutex's metadata to indicate it is unlocked
	// put the thread at the front of this mutex's blocked/waiting queue in to the run queue

	return 0;
};


/* destroy the mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex)
{
	// YOUR CODE HERE
	
	// deallocate dynamic memory allocated during mypthread_mutex_init

	return 0;
};

/* scheduler */
static void schedule()
{
	// YOUR CODE HERE
	
	// each time a timer signal occurs your library should switch in to this context
	
	// be sure to check the SCHED definition to determine which scheduling algorithm you should run
	//   i.e. RR, PSJF or MLFQ
	if(SCHED == 1){
		//run RR
		sched_RR();
	}
	else if(SCHED == 2){
		//run PSJF
		sched_PSJF();
	}
	else{
		//perform FIFO?
	}


	return;
}

/* Round Robin scheduling algorithm */
static void sched_RR()
{
	// YOUR CODE HERE
	
	// Your own implementation of RR
	// (feel free to modify arguments and return types)
	
	return;
}

/* Preemptive PSJF (STCF) scheduling algorithm */
static void sched_PSJF()
{
	// YOUR CODE HERE

	// Your own implementation of PSJF (STCF)
	// (feel free to modify arguments and return types)

	return;
}

/* Preemptive MLFQ scheduling algorithm */
/* Graduate Students Only */
static void sched_MLFQ() {
	// YOUR CODE HERE
	
	// Your own implementation of MLFQ
	// (feel free to modify arguments and return types)

	return;
}

// Feel free to add any other functions you need

// YOUR CODE HERE
