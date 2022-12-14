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


int numFiles = 0;
WO_File *fileData;

int usedMem = 0;
char* diskData;
char* path;

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
		diskData = buf;
		fileData = (WO_File*)diskData+(DISK_SIZE/2);
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

	int index = -1; //find file, if not then return errno
	for(int i=0; i<numFiles; i++){
		WO_File* temp = fileData+i;
		if(strcmp(fileName, temp->name) == 0){ //file found, break out
		index = i;
		break;
		}
	}
	if(index == -1){ //file not found
		errno = ENOENT;
		return errno;
	}
	
	WO_File* file = fileData+index;
	
	if(file->createFlags == WO_RDWR && flags <= WO_RDWR){ //read+write case: anything works
		file->currFlags = flags;
		return file->fd;
	}
	else if(file->createFlags != flags){ //any other case: only allow exactly that flag
		file->currFlags = flags;
		return file->fd;
	}
	
	return -1;
}

int wo_create(char* fileName, int flags){

	if(numFiles == 50){ //if there are 50 files, do not make a file.
		errno = EDQUOT;
		return errno;
	}
	
	for(int i=0; i<numFiles; i++){ //check for duplicate name
		WO_File* temp = fileData+i;
		if(strcmp(fileName, temp->name) == 0){
			errno = EEXIST;
			return errno;
		}
	}
	
	WO_File* newFile = fileData+numFiles; //if no duplicate name, create the file.
	newFile->fd = numFiles;
	newFile->open = 'T';
	strcpy(newFile->name , fileName);
	newFile->createFlags = flags;
	newFile->currFlags = flags;
	
	numFiles+=1; //new file created, increase numFiles
	
	return newFile->fd;
}

int wo_read(int fd, void* buf, int bytes){
	int index = -1;
	for (int i = 0; i < numFiles; i++){ //find the index of the file in opened
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
	for (int i = 0; i < numFiles; i++){ //find the index of the file in opened
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

	if(fd<0 || fd>=numFiles){ //if fd is not legal, return errno
		errno = EBADF;
		return errno;
	}

	WO_File* file = fileData+fd;
	
	file->open = 'F';
	file->currFlags = 0;
	
	return 0;

}

