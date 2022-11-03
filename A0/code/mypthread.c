// File:	mypthread.c

// List all group members' names:
// iLab machine tested on:

	#include <stdatomic.h>
#include "mypthread.h"

// INITAILIZE ALL YOUR VARIABLES HERE
// YOUR CODE HERE



static mypthread_t currThread;
static tcb* currTCB;
static struct sigaction timer;
static struct itimerval interval;
queue* mainQueue = NULL;

static int threadID = 0;

static void schedule();

/* create a new thread */
int mypthread_create(mypthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg)
{
	// printf("\nIN PTHREAD CREATE\n");

	if(threadID == 0){
		currThread = threadID;
		tcb* mainTCB = malloc(sizeof(tcb));

		mainTCB->id = currThread;
		mainTCB->waitingThread = -1;

		mainTCB->status = READY;
		mainTCB->elapsedQuantums = 0;

		mainTCB->valPtr = NULL;
		mainTCB->returnVal = NULL;

		currTCB = mainTCB;
		timer_init(timer, interval);
		threadID++;
		atexit(freeTCBQueue);
		if(getcontext(&(currTCB->context)) == -1){
			perror("Initializing context failed");
			exit(EXIT_FAILURE);
		}
		// //initialize context
		// currTCB->context.uc_link = NULL;
		// //allocate heap space for the thread's stack
		// currTCB->context.uc_stack.ss_sp = malloc(STACK_SIZE);
		// currTCB->context.uc_stack.ss_size = STACK_SIZE;
		// currTCB->context.uc_stack.ss_flags = 0;
		// makecontext(&(currTCB->context), (void*) function, 1, arg);
	}

	// create a Thread Control Block
	currThread = threadID;
	tcb* newTCB = malloc(sizeof(tcb));
	newTCB->id = threadID;
    *thread = threadID;
	newTCB->status = READY;
    newTCB->elapsedQuantums = 0;
    newTCB->waitingThread = -1;
    newTCB->valPtr = NULL;
    newTCB->returnVal = NULL;
	threadID++;
    
	
    if(getcontext(&(newTCB->context)) == -1){
		perror("Initializing context failed");
		exit(EXIT_FAILURE);
	}
	//initialize context
	newTCB->context.uc_link = &currTCB->context;
	//allocate heap space for the thread's stack
	newTCB->context.uc_stack.ss_sp = malloc(STACK_SIZE);
	if(newTCB->context.uc_stack.ss_sp <= 0){
        printf("Memory not allocated: %d\n", newTCB->id);
        exit(EXIT_FAILURE);
    }
	newTCB->context.uc_stack.ss_size = STACK_SIZE;
	newTCB->context.uc_stack.ss_flags = 0;
	makecontext(&(newTCB->context), (void*) function, 1, arg);
	enqueue(newTCB, &mainQueue);

	// after everything is all set, push this thread into the ready queue
	
	// printf("\nTHREAD CREATED\n");
	return 0;
};

/* current thread voluntarily surrenders its remaining runtime for other threads to use */
int mypthread_yield()
{
    // printf("\nIN PTHREAD YIELD\n");
	// YOUR CODE HERE
	
	// change current thread's state from Running to Ready
	// save context of this thread to its thread control block
	// switch from this thread's context to the scheduler's context
	currTCB->status = READY;
	schedule();

	return 0;
};

/* terminate a thread */
void mypthread_exit(void *value_ptr)
{
	// // YOUR CODE HERE

	// // preserve the return value pointer if not NULL
	// // deallocate any dynamic memory allocated when starting this thread
	currTCB->status = FINISHED;

	//if null return value_ptr
    if(currTCB->valPtr == NULL){
        currTCB->returnVal = value_ptr;
    }
    else{
        *currTCB->valPtr = value_ptr;
        currTCB->status = REMOVE;
        removeThread(&mainQueue, currTCB->id);

    }
    readyThreads(&mainQueue, currTCB->id);
    schedule();
	return;
};


/* Wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr)
{
	// // YOUR CODE HERE

	// wait for a specific thread to terminate
	// deallocate any dynamic memory created by the joining thread
    // printf("\nIN PTHREAD JOIN\n");
    //check if thread is finished
    if(isFinished(&mainQueue, thread) == 1){
		// printf("isFinished");
        //thread done
        tcb* temp = getTCB(&mainQueue, thread);
        if(value_ptr != NULL){
            temp->status = REMOVE;
            *value_ptr = temp->returnVal;

            tcb* ptr = temp;
            if(removeThread(&mainQueue, ptr->id) == -1){
                perror("error removeing node, aborting\n");
                abort();
            }
			mypthread_exit(value_ptr);
        }
        // printf("\nPTHREAD JOINED\n");
        return 0;
    }
	//set the thread to waiting
	currTCB->status = WAITING;
	currTCB->waitingThread = thread;

    tcb* temp = getTCB(&mainQueue, thread);
    temp->valPtr = value_ptr;
    schedule();
    // printf("\nPTHREAD WAITING\n");
	return 0;
};


/* initialize the mutex lock */
int mypthread_mutex_init(mypthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr)
{
	// YOUR CODE HERE	
	//initialize data structures for this mutex

	mutex->lock = UNLOCKED;
	mutex->blockedQueue = NULL;
	return 0;

};

/* aquire a mutex lock */
int mypthread_mutex_lock(mypthread_mutex_t *mutex)
{
	// YOUR CODE HERE
	
	// use the built-in test-and-set atomic function to test the mutex
	// if the mutex is acquired successfully, return
	// if acquiring mutex fails, put the current thread on the blocked/waiting list and context switch to the scheduler thread
	
	//atomically test lock status
	while(atomic_flag_test_and_set(&(mutex->lock)) == LOCKED)
	{		
		currTCB->status = BLOCKED;
		//add to mutex blocked queue
		enqueue(currTCB, &(mutex->blockedQueue));			
		//remove thread from main queue
		// currTCB->status = REMOVE;
		// removeThread(&mainQueue, currTCB->id);
		schedule();
	}
	//lock acquired
	return 0;

};

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex)
{
	// YOUR CODE HERE	
	
	// update the mutex's metadata to indicate it is unlocked
	// put the thread at the front of this mutex's blocked/waiting queue in to the run queue
	mutex->lock = UNLOCKED;
	
	queue* next = mutex->blockedQueue;
	//if blocked threads are waiting, ready first in queue
	if(next != NULL)
	{
		next->TCB->status = READY;	
		updateQ(&mainQueue, next->TCB->id);
		// enqueue(next->TCB, &mainQueue);
		dequeue(&(mutex->blockedQueue));
	}
	
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
void schedule()
{
	// YOUR CODE HERE
	// printf("called schedule()");	
	// each time a timer signal occurs your library should switch in to this context
	
	// be sure to check the SCHED definition to determine which scheduling algorithm you should run
	//   i.e. RR, PSJF or MLFQ
	if (SCHED == 1){
		//run RR
		sched_RR();
	}
	else if (SCHED == 2){
		sched_PSJF();
	}


	return;
}

/* Round Robin scheduling algorithm */
static void sched_RR()
{
	// printf("called sched_RR");
	//ignore alarm during scheduling
    	//signal(SIGALRM, SIG_IGN);

	tcb* prevTCB = currTCB;

	currTCB = dequeue(&mainQueue);
	//if queue is empty
	if(currTCB == NULL){
		currTCB = prevTCB;
		return;
	}
	enqueue(prevTCB, &mainQueue);
	//restart timer
	timer_init(timer, interval);
	// printf("before\n");
	//switch context
	if(swapcontext(&(prevTCB->context),&(currTCB->context)) == -1){
		printf("swapcontext error\n");
	}
	// printf("\nSCHEDULE END\n");
	return;
}

/* Preemptive PSJF (STCF) scheduling algorithm */
static void sched_PSJF()
{
	// printf("called sched_PSJF");
    // printf("\nIN SCHEDULE\n");

	//ignore alarm during scheduling
    signal(SIGALRM, SIG_IGN);

	tcb *prevTCB = currTCB;

	currTCB = dequeue(&mainQueue);
	//if queue is empty
	if(currTCB == NULL){
		currTCB = prevTCB;
		return;
	}
	prevTCB->elapsedQuantums++;
    enqueue(prevTCB, &mainQueue);
	//restart timer
	timer_init(timer, interval);

	//switch context
	swapcontext(&(prevTCB->context),&(currTCB->context));
	
    // printf("\nSCHEDULE END\n");
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
void printThreadQueue(queue** q){
    //have to modify this
    printf("PRINT THREAD QUEUE\n");
    printf("Current TCB\n");
    if(currTCB == NULL){
        printf("No Current TCB\n");
        return;
    }
    else{
        printf("Thread ID: %d\n",  currTCB->id);
        printf("Thread Status: %d\n", (int) currTCB->status);
        printf("Thread Elapsed Quantums (Runtime): %d\n\n",
                 currTCB->elapsedQuantums);
    }
    printf("THREAD QUEUE\n");
    queue* ptr = *q;

    if(ptr == NULL){
        printf("Queue is empty or NULL\n");
        return;
    }

    int count = 1;
    for(ptr; ptr != NULL; ptr = ptr->next){
    
        printf("Node %d.\n", count);
        printf("Thread ID: %d\n",  ptr->TCB->id);
        printf("Thread Status: %d\n", (int) ptr->TCB->status);
        printf("Waiting Thread: %d\n", ptr->TCB->waitingThread);
        printf("Thread Elapsed Quantums (Runtime): %d\n\n",
                 ptr->TCB->elapsedQuantums);

        printf("Return Value: %p\n", ptr->TCB->returnVal);
        printf("Value Pointer: %p\n", ptr->TCB->valPtr);

        count++;
    }
    printf("LIST DONE\n");
}
/*
inserts thread into queue 
(for PSJF: makes into priority queue)
(for RR: just inserts at the end)
*/
void enqueue(tcb* thread, queue** argQ){
    // printf("\nENQUEING THREAD\n");
    // printThreadQueue(argQ);
	int thisQuantum = thread->elapsedQuantums;
	//queue is null
	if(argQ == NULL){
		return;
	}
	
	//thread to insert
	queue* newThread = malloc(sizeof(queue));
	newThread->TCB = thread;
	newThread->next = NULL;
	newThread->prev = NULL;

	//reference to head
	queue *head = *argQ;
	
	//queue is empty
	if(head == NULL){
		*argQ = newThread;
		// printf("\nFINISHED ENQUEING THREAD\n");
		// printThreadQueue(argQ);
		return;
	}
	//if the thread is less than the lowest quantum thread (the head of queue)
	else if(head->TCB->elapsedQuantums > thisQuantum && SCHED == 2){
		newThread->next = head;
		head->prev = newThread;
		*argQ = newThread;
		// printf("\nFINISHED ENQUEING THREAD\n");
		// printThreadQueue(argQ);
		return;
	}

	queue* ptr = head;
	queue* ptr2 = head->next;
	//for RR
	if (SCHED == 1){
		while (ptr2 != NULL){
			ptr = ptr2;
			ptr2 = ptr2->next;
		}
	}
	//for PSJF
	else{
		//find the next thread that is grater than this thread's quantum
		while(ptr2 != NULL && ptr2->TCB->elapsedQuantums <= thisQuantum){
			ptr = ptr2;
			ptr2 = ptr2->next;
		}
	}
	newThread->next = ptr2;
	if(ptr2 != NULL){
		ptr2->prev = newThread;
	}
	ptr->next = newThread;
	newThread->prev = ptr;	

    // printf("\nFINISHED ENQUEING THREAD\n");
	// printThreadQueue(argQ);
}

//returns NULL if queue is empty
tcb* dequeue(queue** argQ){
    // printf("\nDEQUEING THREAD\n");
	// printThreadQueue(argQ);
	//reference to the head of the queue
	queue* temp = *argQ;

	//queue is empty
	if(temp == NULL){
		return NULL;
	}

	//the TCB we are returning
	tcb* returnTCB = NULL;

	// only dequeues the next thread with status READY
	for (queue* ptr = *argQ; ptr != NULL; ptr = ptr->next){
		if(ptr->TCB->status == READY){
			//if at the front of the queue
			if (ptr == temp){
				returnTCB = ptr->TCB;
				*argQ = ptr->next;
				ptr->next = NULL;
				free(ptr);
				break;
			}
			//if in the middle of the queue
			else if (ptr->next != NULL){
				returnTCB = ptr->TCB;
				ptr->prev->next = ptr->next;
				ptr->next->prev = ptr->prev;
				ptr->next = NULL;
				ptr->prev = NULL;
				free(ptr);
				break;
			}
			//if at the end of the queue
			else{
				returnTCB = ptr->TCB;
				ptr->prev->next = NULL;
				ptr->prev = NULL;
				free(ptr);
				break;
			}
		}
	}
    // printf("\nFINISHED DEQUEING THREAD\n");
	// printThreadQueue(argQ);
	return returnTCB;
}

//initialize timer
void timer_init(struct sigaction timer, struct itimerval interval){

    memset(&timer, 0, sizeof(timer));

	//timer calls schedule when it goes off, no need for sighandler or scheduler context
    timer.sa_handler = &schedule;
    // sigset_t maskSet;
    // sigaddset(&maskSet, SIGALRM);
    // timer.sa_mask = maskSet;

    if(sigaction(SIGALRM, &timer, NULL) < 0){
        perror("SIGACTION failed\n");
        exit(EXIT_FAILURE);
    }

    //initial timer expiration at QUANTUM
    interval.it_value.tv_sec = 0;
    interval.it_value.tv_usec = QUANTUM;

    //set interval to QUANTUM
    interval.it_interval = interval.it_value;
    //start timer
    setitimer(ITIMER_REAL, &interval, NULL);
}

//returns the TCB that has the same id as the argument thread
tcb* getTCB(queue** argQ, mypthread_t thread){
    tcb* temp = NULL;
    for(queue* ptr = *argQ; ptr != NULL; ptr = ptr->next){
        if(ptr->TCB->id == thread){
            temp = ptr->TCB;
            break;
        }
    }
    return temp;
}

//changes status to all threads waiting for argument thread to READY
void readyThreads(queue** argQ, mypthread_t thread){
    for(queue* ptr = *argQ; ptr != NULL; ptr = ptr->next){
        if(ptr->TCB->status == WAITING && ptr->TCB->waitingThread == thread){
            ptr->TCB->waitingThread = -1;
            ptr->TCB->status = READY;
        }
    }
    return;
}

//frees all TCB in the queue
void freeTCBQueue(void){
    queue* ptr = mainQueue;

    while(ptr != NULL){
        queue* temp = ptr;
        ptr = ptr->next;
        freeThread(temp);
    }
    free(currTCB);
}

//frees given head(TCB) from Queue
void freeThread(queue* argQ){
    free(argQ->TCB->context.uc_stack.ss_sp);
    free(argQ->TCB);
    free(argQ);
}

//checks if the argument thread has the status FINISHED
int isFinished(queue** argQ, mypthread_t thread){
    int isFinished = 0;
    for(queue* ptr = *argQ; ptr != NULL; ptr = ptr->next){
        if(ptr->TCB->status == FINISHED && ptr->TCB->id == thread){
            isFinished = 1;
            break;
        }
    }
    return isFinished;
}

//removes argument threads from the Queue
int removeThread(queue** argQ, mypthread_t thread){
    int remove = -1;
    for(queue* ptr = *argQ; ptr != NULL; ptr = ptr->next){
        if(ptr->TCB->status = REMOVE && ptr->TCB->id == thread){
            queue* temp = ptr;
            if(temp->next != NULL){
                temp->next->prev = temp->prev;
            }
            if(temp->prev != NULL){
                temp->prev->next = temp->next;
            }
            temp->next = NULL;
            temp->prev = NULL;
            freeThread(temp);
            remove = 1;
            break;
        }
    }
    return remove;
}

void updateQ(queue** argQ, mypthread_t thread){
	for(queue* ptr = *argQ; ptr != NULL; ptr = ptr->next){
        if(ptr->TCB->id == thread){
            ptr->TCB->status = READY;
            break;
        }
    }
}