#ifndef MAMOERY_H
#define MAMOERY_H

#include <stddef.h>
// for: size_t

void* memoryAlloc(size_t size);
void  memoryFree(void* mem);
void* memoryRealloc(void* mem, size_t size);

char* memoryStringnDup(const char *str);
char* memoryStringnLengthDup(const char *str, size_t length);

#endif
