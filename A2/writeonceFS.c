// File: writeonceFS.c
//
// Group Members: Max Humes, Lorenzo Ladao, Steven Zyontz 

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "writeonceFS.h"

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

int wo_unmount(void* addr){
	return 0;
}

int wo_open(char* fileName, int flags[]){
	return 0;
}

int wo_read(int fd, void* buf, int bytes){
	return 0;
}

int wo_write(int fd, void* buf, int bytes){
	return 0;
}

int wo_close(int fd){
	return 0;
}
