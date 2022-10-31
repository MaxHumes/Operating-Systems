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

//initialize timer
void* DoStuff() {

  printf("Test.\n");

}

void testScheduler()
{
	mypthread_t t1;
	mypthread_create(&t1, NULL, &DoStuff, NULL);	
	printf("thread created\n");
}

int main(int argc, char **argv) {

	/* Implement HERE */
	/*
	//for setting itimer
	struct itimerval it_val; 
	struct sigaction timer;

    	memset(&timer, 0, sizeof(timer));

    	timer.sa_handler = (void (*)(int))DoStuff;
  	if(sigaction(SIGALRM, &timer, NULL) < 0){
        //signal handler/sigaction failed
        perror("SIGACTION failed\n");
        exit(EXIT_FAILURE);
    	}
	*/
    	/* Upon SIGALRM, call DoStuff().
    	* Set interval timer.  We want frequency in ms, 
    	* but the setitimer call needs seconds and useconds. */
    	// if (signal(SIGALRM, (void (*)(int)) DoStuff) == SIG_ERR) {
    	//     perror("Unable to catch SIGALRM");
    	//     exit(1);
    	// }
	/*
    	it_val.it_value.tv_sec =     INTERVAL/1000;
    	it_val.it_value.tv_usec =    (INTERVAL*1000) % 1000000;   
    	it_val.it_interval = it_val.it_value;
    	if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
        	perror("error calling setitimer()");
        	exit(1);
    	}*/
	
	testScheduler();
	return 0;
}

