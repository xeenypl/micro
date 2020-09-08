#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <string.h>
#include <stdint.h>

struct String {
    char*  string;
    size_t length;
};

static inline struct String newString(const char* str) {
    return (struct String) {
        .string = (char*)str,
        .length = strlen(str)
    };
}

static inline struct String newStringL(const char* str, size_t len) {
    return (struct String) {
        .string = (char*)str,
        .length = len
    };
}

#endif
