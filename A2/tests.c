#include<stdio.h>
#include<stdlib.h>
#include "writeonceFS.h"

int main(int argc, char* argv[])
{
	char* buf;
	buf = malloc(DISK_SIZE);

	wo_mount("disk.data", buf);
	int fd = wo_create("testfilee.txt", WO_RDWR);
	
	char* stuff = "this is some bs I am trying to write to a file";
	wo_write(fd, stuff, 20); 	

	wo_unmount(buf);
}
