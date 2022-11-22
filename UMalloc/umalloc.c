#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umalloc.h"

static char block[MEM_SIZE];
char init = 'F';


void* umalloc(size_t size, const char* fileName, int lineNumber){

  if(size<=0||size>=MEM_SIZE-sizeof(META)){ //if attempting to grab too little/too much memory, ERROR
    printf("Attempt to allocate illegal amount of memory in %s, line %d.\n", fileName, lineNumber);
    return NULL;
  }

  if(init=='F'){ //If not initialized, initialize with a header.
    initialize();
  }
  
  char* curr = &block[0]; //pointer that will go to every single block
  META skipped = 0; //total size of blocks ignored when allocating, used for errors
  
  while(curr <= &block[MEM_SIZE-1]){ //end when pointer is out of bounds
  
  	if(getStatus(curr)=='F'){ //if in use, skip over to next metadata
  		curr += getSize(curr)+sizeof(META);
  		continue;
  	}
  	else if(getStatus(curr)=='T'){
  		
  		if(getSize(curr)<size){ //if block not big enough, add to skipped, continue
  			skipped+=getSize(curr);
  			curr+= getSize(curr)+sizeof(META);
  			continue;
  		}
  		else if(getSize(curr)==size||getSize(curr)-size<(sizeof(META)+1)){ //block fits perfectly OR not enough room to make new head with at least one byte of space, set block to in use and return pointer
  			setStatus(curr, 'F');
  			//printf("Allocated block of size %u.\n",getSize(curr));
  			return (void*)curr+sizeof(META);
  		}
  		else if(getSize(curr)-size>=(sizeof(META)+1)){ //block fits, with enough room for a new head.
  			char* newHead = curr+sizeof(META)+size;
  			setSize(newHead,getSize(curr)-size-sizeof(META));
  			setStatus(newHead,'T');
  			
  			setSize(curr,size);
  			setStatus(curr,'F');
  			//printf("Allocated block of size %u.\n",getSize(curr));
  			return (void*)curr+sizeof(META);
  		}
  		
  	}//end Status == 'T'
  
  }//end block loop

	if(skipped==0){
		printf("No free memory available. Allocation attempted at %s, line %d.\n",fileName,lineNumber);
		return NULL;
	}
	else if(skipped < size){
		printf("Not enough memory for allocation. Allocation attempted at %s, line %d.\n",fileName,lineNumber);
		return NULL;
	}
	else if(skipped >= size){
		printf("External fragmentation error. Allocation attempted at %s, line %d.\n",fileName,lineNumber);
		return NULL;
	}


  return NULL;
  
}//end umalloc

void ufree(void* ptr, const char* fileName, int lineNumber){

	if((char *)ptr < &block[0] || (char *)ptr > &block[MEM_SIZE-1]){ //ptr not in block, so not from malloc
	printf("Attempt to free() non-allocated memory at %s, line %d.\n",fileName,lineNumber);
	return;
	}

	char* curr = &block[0];
	char* prev = NULL;
	
	while(curr<=&block[MEM_SIZE-1]){ //while curr in block, look for header
	
	if(curr+sizeof(META)<(char*)ptr){ //not at ptr yet, so continue
	
		prev=curr;
		curr+=getSize(curr)+sizeof(META);
		continue;
	}
	else if(curr+sizeof(META)>(char*)ptr){//passed the ptr, so something must be wrong
	
	char* testPtr = ((char*)ptr)-sizeof(META);
	if( (getStatus(testPtr)=='T' || getStatus(testPtr)=='F') && getSize(testPtr)>=1 && getSize(testPtr)<=MEM_SIZE-sizeof(META)){ //ptr was a valid head, but was passed, so throw repetitive free error
	
	 printf("Redundant free() operation performed at %s, line %d.\n",fileName,lineNumber);
	 return;
	}
	else{ //ptr not a valid head, so throw free at not start of block
	
		printf("Attempt to free() at non-start-of-block at %s, line %d.\n",fileName,lineNumber);
		return;
	}
	}//end passed ptr check
	
	else if(curr+sizeof(META)==(char*)ptr && getStatus(curr)=='T'){ //found ptr, but already freed, so rep free error
	
		printf("Redundant free() operation performed at %s, line %d.\n",fileName,lineNumber);
	 	return;
	}
	else if(curr+sizeof(META)==(char*)ptr && getStatus(curr)=='F'){//found ptr, and its not been freed
	
	setStatus(curr,'T');
	//printf("Freeing block of size %u\n",getSize(curr));

	//coalesce curr and next block if both are free	
	char* next = curr+sizeof(META)+getSize(curr);
	if(next<&block[MEM_SIZE]){//check if next is in bounds (exists)
		if(getStatus(next)=='T'){//next is free, so coalesce
			setSize(curr,getSize(curr)+sizeof(META)+getSize(next));
		}
	}
	
	//coalesce curr and prev block if both are free
	
	if(prev!=NULL){ //if prev exists, check if it was free
	
		if(getStatus(prev)=='T'){ //if free, give prev all the space curr has.
			setSize(prev,getSize(prev)+sizeof(META)+getSize(curr));
		}
	}
	return;
	}//end found ptr case
	
	prev=curr;
	curr+=getSize(curr)+sizeof(META);
	
	}//end while loop
	
	char* testPtr = ((char*)(ptr))-sizeof(META);
	
	if( (getStatus(testPtr)=='T' || getStatus(testPtr)=='F') && getSize(testPtr)>=1 && getSize(testPtr)<=MEM_SIZE-sizeof(META)){ //ptr was a valid head, but was passed, so throw repetitive free error
	
	 printf("Redundant free() operation performed at %s, line %d.\n",fileName,lineNumber);
	 return;
	}


  return;
}//end ufree

void initialize(){
  setStatus(&block[0],'T');
  init='T';
  setSize(&block[0],(META)MEM_SIZE-sizeof(META));
  //printf("META %c\n",getStatus(&block[0]));
  //printf("SIZE %u\n",getSize(&block[0]));
}

char getStatus(char* ptr){

  return *(ptr+sizeof(META)-1);
  
}

void setStatus(char* ptr, char status){

  *(ptr+sizeof(META)-1) = status;
  return;
}

META getSize(char* ptr){

  META* val = (META*)(ptr);
  //printf("GET SIZE: %u\n",*val);
  return *val & 0x00FFFFFF;
  
}

void setSize(char* ptr, META size){
  //printf("REQUEST SIZE: %u\n",size);
  META* head = (META*)ptr;
  char status = getStatus(ptr);
  *head=size;
  setStatus(ptr,status);
  return;
}
