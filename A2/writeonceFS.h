// File: writeonceFS.h
//
// Group Members: Max Humes, Lorenzo Ladao, Steven Zyontz

#define DISK_SIZE 4000000
#define BLOCK_SIZE 1000

#define WO_RDONLY 0
#define WO_WRONLY 1
#define WO_RDWR 2

#define WO_CREAT 3

typedef struct LinkedList{
	int index;
	void* buffer;
	struct LinkedList* nextBlock;
} LinkedList;
typedef struct WO_File{
	int fd;
	int flags;
	LinkedList* blocks;
} WO_File;

int wo_mount(char* fileName, void* buf);

int wo_unmount(void* buf);

int wo_open(char* fileName, int flags);

int wo_create(char* fileName, int flags);

int wo_read(int fd, void* buf, int bytes);

int wo_write(int fd, void* buf, int bytes);

int wo_close(int fd);
