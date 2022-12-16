#include<stdio.h>
#include<stdlib.h>
#include "writeonceFS.h"

int main(int argc, char* argv[])
{
	char* buf;
	
	wo_mount("disk.data", buf);
	wo_create("testfile.txt", WO_RDWR);
	
	wo_unmount(buf);
}
