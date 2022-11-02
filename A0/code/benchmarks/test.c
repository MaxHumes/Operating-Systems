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
#include <pthread.h>
#include "../mypthread.h"
#include "../mypthread.c"

/* A scratch program template on which to call and
 * test mypthread library functions as you implement
 * them.
 *
 * You can modify and use this program as much as possible.
 * This will not be graded.
 */
#define INTERVAL 5000

static struct itimerval interval;
int counter;
queue* Queue = NULL;

//initialize timer
void* DoStuff() {
	printf("starting thread\n");
	for(int i = 0; i < 10000; i++)
	{
		counter++;
	}
	printf("done");
}

void testScheduler()
{
	pthread_t t1, t2;
	// t1 = malloc(sizeof(mypthread_t));
	pthread_create(&t1, NULL, &DoStuff, NULL);
	pthread_create(&t2, NULL, &DoStuff, NULL);	
	printf("thread created\n");
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	printf("thread joined");


	printf("Counter Value: %d", counter);

}

int main(int argc, char **argv) {

	/* Implement HERE */
	testScheduler();
	
	//Code to test timer init
	
	// timer_init(timer, interval);
	// for( ; ; )
	// {
	// 	pause();
	// }
	// return 0;
	
}

