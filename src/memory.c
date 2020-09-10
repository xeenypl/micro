#include <stdio.h>
// for: perror
#include <stdlib.h>
// for: calloc, free, realloc, exit EXIT_FAILURE
#include <string.h>
// for: strdup, strndup

#include "memory.h"

void* memoryAlloc(size_t size) {
    void* res = calloc(1, size);
    if (res == NULL) {
        perror("Memory Error");
        exit(EXIT_FAILURE);
    }
    return res;
}

void* memoryRealloc(void* mem, size_t size) {
    void* res = realloc(mem, size);
    if (res == NULL) {
        perror("Memory Error");
        exit(EXIT_FAILURE);
    }
    return res;
}

void memoryFree(void* mem) {
    free(mem);
}

char* memoryStringnDup(const char *str) {
    void* res = strdup(str);
    if (res == NULL) {
        perror("Memory Error");
        exit(EXIT_FAILURE);
    }
    return res;
}

char* memoryStringnLengthDup(const char *str, size_t length) {
    void* res = strndup(str, length);
    if (res == NULL) {
        perror("Memory Error");
        exit(EXIT_FAILURE);
    }
    return res;
}
