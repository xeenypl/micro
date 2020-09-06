#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>

struct Args {
    int   verbose;
    char* output;
};

extern struct Args args;

#endif
