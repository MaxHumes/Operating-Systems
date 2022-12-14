#include<stdio.h>
#include<stdlib.h>
#include "writeonceFS.h"

int main(int argc, char* argv[])
{
	char* buf = malloc(DISK_SIZE);
	buf[0] = 'b';
	buf[1] = 'i';
	buf[2] = 't';
	buf[3] = 'c';
	buf[4] = 'h';

	wo_unmount(buf);
}
