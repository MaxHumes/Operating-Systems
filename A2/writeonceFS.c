// File: writeonceFS.c
//
// Group Members: Max Humes, Lorenzo Ladao, Steven Zyontz 

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "writeonceFS.h"

WO_File *opened[100]; 
WO_File *closed[100];
int numFilesOpen = 0;
int numFilesClosed = 0;

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
	return 0;
}

int wo_write(int fd, void* buf, int bytes){
	return 0;
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
