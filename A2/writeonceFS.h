// File: writeonceFS.h
//
// Group Members: Max Humes, Lorenzo Ladao, Steven Zyontz

#define DISK_SIZE 4000000
#define BLOCK_SIZE 1000

#define WO_RDONLY 1
#define WO_WRONLY 2
#define WO_RDWR 4



typedef struct WO_File{
	int fd; //file descriptor id, numbered in order of creation
	char open; //T if file is open, F if closed. Defaults to open on create.
	char name[16]; //name of file
	int createFlags; //flags file was created with
	int currFlags; //flags fi2le was opened with.
} WO_File;

int wo_mount(char* fileName, void* buf);

int wo_unmount(void* buf);

int wo_open(char* fileName, int flags);

int wo_create(char* fileName, int flags);

int wo_read(int fd, void* buf, int bytes);

int wo_write(int fd, void* buf, int bytes);

int wo_close(int fd);
