// File: writeonceFS.c
//
// Group Members: Max Humes, Lorenzo Ladao, Steven Zyontz 

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "writeonceFS.h"

WO_File *opened[100]; 
WO_File *closed[100];
int numFilesOpen = 0;
int numFilesClosed = 0;
int usedMem = 0;

/*
WO_File *newFile;
	newFile = malloc(sizeof(WO_File));
	newFile->fd = fd;
	newFile->flags = flags;
*/


//read disk with fileName to buf
int wo_mount(char* fileName, void* buf){
	buf = malloc(DISK_SIZE);

	//open disk file
	FILE* fp = fopen(fileName, "r");
	if(fp != NULL)
	{
		fread(buf, sizeof(char), DISK_SIZE, fp);
		if(ferror(fp)){
			return errno;
		}
		fclose(fp);
	}
	else
	{
		return errno;
	}
	
	return 0;
}

int wo_unmount(void* buf){
	//open file on OS
	FILE* fp = fopen("disk.data", "w");
	if(fp != NULL)
	{
		fwrite(buf, sizeof(char), DISK_SIZE, fp);
		if(ferror(fp)){
			return errno;
		}
		fclose(fp);
	}
	else
	{
		return errno;
	}
	return 0;
}

int wo_open(char* fileName, int flags){

	int mode = -1; //use access to check if file exists and can be opened with perms
	if(flags == WO_RDONLY){
	mode = R_OK;
	}
	else if(flags == WO_WRONLY){
	mode = W_OK;
	}
	else if(flags == WO_RDWR){
	mode = R_OK|W_OK;
	}
	if(access(fileName,mode) == 0){//file exists and permissions are allowed
		
		int openFlag = -1; //convert to open(2) flags
		if(mode == R_OK){
		openFlag = O_RDONLY;
		}
		else if(mode == W_OK){
		openFlag = O_WRONLY;
		}
		else if(mode == (R_OK|W_OK) ){
		openFlag = O_RDWR;
		}
		
		int fd = open(fileName, openFlag);
		
		if(fd==-1){
		return errno;
		}
		
		WO_File *newFile;
		newFile = malloc(sizeof(WO_File));
		newFile->fd = fd;
		newFile->flags = openFlag;
		
		opened[numFilesOpen] = newFile;
		numFilesOpen+=1;
		
	}
	else{//file does not exist and/or file permissions are not allowed
		return errno;
	}
	return 0;
}

int wo_create(char* fileName, int flags){

	if(access(fileName,F_OK) != 0){//file does not exist
		
		int openFlag = -1; //convert to open(2) flags
		if(flags == WO_RDONLY){
		openFlag = O_RDONLY;
		}
		else if(flags == WO_WRONLY){
		openFlag = O_WRONLY;
		}
		else if(flags == WO_RDWR ){
		openFlag = O_RDWR;
		}
		
		int fd = open(fileName, openFlag, O_CREAT);
		
		if(fd==-1){
		return errno;
		}
		
		WO_File *newFile;
		newFile = malloc(sizeof(WO_File));
		newFile->fd = fd;
		newFile->flags = openFlag;
		
		opened[numFilesOpen] = newFile;
		numFilesOpen+=1;
		
	}
	else{//file does not exist and/or file permissions are not allowed
		return errno;
	}
	return 0;
}

int wo_read(int fd, void* buf, int bytes){
	int index = -1;
	for (int i = 0; i < numFilesOpen; i++){ //find the index of the file in opened
		if (fd == opened[i]->fd){
			index = i;
			break;
		}
	}

	if (index == -1) //if not found, return errno
		return errno;

	return read(opened[index]->fd, buf, bytes);
}
void append(LinkedList** head, char* buf, int index, int size){
	LinkedList* newBlock = malloc(sizeof(LinkedList));
	LinkedList* last = *head;

	newBlock->buffer = malloc(BLOCK_SIZE);
	memcpy(newBlock->buffer, buf, size);
	
	newBlock->index = index;
	newBlock->nextBlock = NULL;

	if (*head == NULL){
		*head = newBlock;
		return;
	}

	while(last->nextBlock != NULL){
		last = last->nextBlock;
	}
	last->nextBlock = newBlock;
	return;
}
int wo_write(int fd, void* buf, int bytes){
	int index = -1;
	for (int i = 0; i < numFilesOpen; i++){ //find the index of the file in opened
		if (fd == opened[i]->fd){
			index = i;
			break;
		}
	}

	if (index == -1) //if not found, return errno
		return errno;

	opened[index]->blocks = NULL;
	int writeBytes = 0;
	int remainingBytes = bytes;
	int i = 0;
	while (remainingBytes > 0){
		
		int count = bytes;
		if (usedMem >= DISK_SIZE){	//if written memory has reached the threshold of disk size
			printf("Not enough memory available.");
			break;
		}
		else if (usedMem + bytes > DISK_SIZE){	//if write goes over disk size, write data partially
			count = DISK_SIZE - usedMem;
		}
		if (bytes > BLOCK_SIZE)	//set the bytes being written to the max block size
			count = BLOCK_SIZE;
		
		if (strlen(buf) < count){
			count = strlen(buf) + 1;
			remainingBytes = count;
		}
		append(&opened[index]->blocks + (i * DISK_SIZE), buf, index, count); 
		writeBytes += count;
		usedMem += count;
		remainingBytes -= count;
		i++;
	}
	
	return writeBytes;
}


int wo_close(int fd){


	int index=-1;
	for(int i=0; i<numFilesOpen; i++){ //find the index of the file in opened
		if(fd==opened[i]->fd){
			index=i;
			break;
		}
	}
	if(index == -1){ //if not found, return errno
	return errno;
	}
	
	closed[numFilesClosed] = opened[index]; //add file to closed
	opened[index] = NULL; //remove file from opened
	
	numFilesClosed+=1; //one more closed file
	
	for(int i=index; i<99; i++){ //shift every opened file one position left
	opened[i] = opened[i+1];
	}

	return 0;
}

