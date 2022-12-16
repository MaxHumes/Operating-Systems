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
void* diskPath;

int totalBlocks = 100;
unsigned short* owners;
int numBlocks = 0;


//read disk with fileName to buf
int wo_mount(char* fileName, void* buf){
	buf = malloc(DISK_SIZE);

	char newDisk = 'F';
	//check if disk exists on native OS
	if(access(fileName, F_OK) != 0)
	{
		diskPath = fileName;
		wo_unmount(buf);
	       	newDisk = 'T';	
	}

	//open disk file for reading
	FILE* fp = fopen(fileName, "r");
	if(fp != NULL)
	{
		fread(buf, sizeof(char), DISK_SIZE, fp);
		if(ferror(fp)){
			return errno;
		}
		diskData = buf;
		owners = (unsigned short*)(diskData + DISK_SIZE - sizeof(unsigned short) * totalBlocks);
		fileData = (WO_File*)diskData+((DISK_SIZE/sizeof(WO_File))/2);
		diskPath = fileName;
		
		if(newDisk == 'F')
		{
			numFiles = *(((int*)diskData) + ((DISK_SIZE - (DISK_SIZE/4))/sizeof(int)));
		}
		else
		{
			numFiles = 0;
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
	if(diskPath == NULL)
	{
		errno = EINVAL;
		return errno;
	}

	//open file on OS
	FILE* fp = fopen(diskPath, "w");
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
	*(((int*)diskData) + ((DISK_SIZE - (DISK_SIZE/4))/sizeof(int))) = numFiles; 

	return newFile->fd;
}

int wo_read(int fd, void* buf, int bytes){
	if(fd<0 || fd>=numFiles){ //if fd is not legal, return errno
		errno = EBADF;
		return errno;
	}

	WO_File* file = fileData+fd;
	if (file->open == 'F' ||file->currFlags == WO_WRONLY){	//check permissions
		errno = EACCES;
		return errno;
	}	

	int readBytes = 0;
	int remainingBytes = bytes;
	for (int i = 0; i < totalBlocks; i++){

		int count = remainingBytes;
		if (count == 0)
			break;
		if (count > BLOCK_SIZE){
			count = BLOCK_SIZE;
		}
		if (owners[i] == fd){
			char* src = diskData + i * BLOCK_SIZE;
			// memcpy(buf, dest, BLOCK_SIZE + 1);
			memcpy(buf + readBytes, src, count);
		}
		remainingBytes -= count;
		readBytes += count;
	}
	
	return readBytes;
}

int wo_write(int fd, void* buf, int bytes){
	if(fd<0 || fd>=numFiles){ //if fd is not legal, return errno
		errno = EBADF;
		return errno;
	}

	WO_File* file = fileData+fd;
	if (file->open == 'F' || file->currFlags == WO_RDONLY){	//check permissions
		errno = EACCES;
		return errno;
	}

	int writeBytes = 0;
	int remainingBytes = bytes;
	int i = 0;
	while (remainingBytes > 0){
		
		int count = remainingBytes;
		//if written memory has reached the threshold of disk size
		if (diskData[DISK_SIZE/2] != '\0'){	//1999 is final block. if used, no free blocks left
			printf("Not enough memory available.");
			break;
		}

		char* data = diskData;

		while(data[i * BLOCK_SIZE] != '\0'){	//find next free block
			i++;
		}
		
		if (count > BLOCK_SIZE)	//set the bytes being written to the max block size
			count = BLOCK_SIZE;

		char* src = diskData + (i * BLOCK_SIZE);
		memcpy(src, buf + writeBytes, count);
		owners[numBlocks] = fd; 
		numBlocks++;
		writeBytes += count;
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



