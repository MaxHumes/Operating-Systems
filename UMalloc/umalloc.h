#ifndef __UMALLOC_H__
#define __UMALLOC_H__

#define malloc( s ) umalloc( s, __FILE__, __LINE__ )
#define free( p ) ufree( p, __FILE__, __LINE__ )

#include <stdlib.h>
#include <stdio.h>

#define MEM_SIZE 10000000 //memory block size

typedef unsigned int META;

void* umalloc(size_t size, const char* fileName, int lineNumber);

void ufree(void* ptr, const char* fileName, int lineNumber);

void initialize();

char getStatus(char* ptr);

void setStatus(char* ptr, char status);

META getSize(char* ptr);

void setSize(char* ptr, META size);

#endif
