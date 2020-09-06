#include <stdio.h>
// for: fopen, fseek, fclose, SEEK_END, SEEK_SET, perror
#include <stdlib.h>
// for: exit, EXIT_FAILURE

#include "memory.h"

const char* readFile(const char* path) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        perror("File error!");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    size_t lengh = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *string = memoryAlloc(lengh + 1);
    if (fread(string, sizeof(char), lengh, file) != lengh) {
        perror("File error!");
        exit(EXIT_FAILURE);
    }

    fclose(file);
    return string;
}
