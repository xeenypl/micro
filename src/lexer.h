#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "string.h"

extern size_t current_line_number;
extern const char* current_file;

void skipWhiteSpaces(const char** src);

bool matchUint(const char* src, const char** end, uint64_t* result);
bool matchFloat(const char* src, const char** end, long double* result);

bool matchUpperName(const char* src, const char** end, struct String* result);  // [A-Z][a-zA-Z0-9]*
bool matchLowerName(const char* src, const char** end, struct String* result);  // [a-z][a-zA-Z0-9]*
bool matchDotName(const char* src, const char** end, struct String* result);    // .[a-zA-Z0-9]+
bool matchAtName(const char* src, const char** end, struct String* result);     // @[a-zA-Z0-9]+
bool matchString(const char* src, const char** end, struct String* result);     // "[^"]*"

bool matchKeyword(const char* src, const char** end, const char* str);

static inline bool matchChar(const char* src, const char** end, char c);

static inline bool matchAsign(const char* src, const char** end);            // =
static inline bool matchEqual(const char* src, const char** end);            // ==
static inline bool matchNotEqual(const char* src, const char** end);         // !=
static inline bool matchLogicalNot(const char* src, const char** end);       // !

static inline bool matchLessThen(const char* src, const char** end);         // <
static inline bool matchLessThenOrEqual(const char* src, const char** end);  // >= 
static inline bool matchLeftShift(const char* src, const char** end);        // <<
static inline bool matchGreatThen(const char* src, const char** end);        // >
static inline bool matchGreatThenOrEqual(const char* src, const char** end); // >=
static inline bool matchRightShift(const char* src, const char** end);       // >>

static inline bool matchBitwizeOr(const char* src, const char** end);        // |
static inline bool matchLogicalOr(const char* src, const char** end);        // ||
static inline bool matchBitwizeAnd(const char* src, const char** end);       // &
static inline bool matchLogicalAnd(const char* src, const char** end);       // &&

static inline bool matchChar(const char* src, const char** end, char c) {
    skipWhiteSpaces(&src);
    if ((*src) == c) {
        if (end != NULL) {
            (*end) = src + 1;
        }
        return true;
    }
    return false;
}

static inline bool matchAsign(const char* src, const char** end) {
    skipWhiteSpaces(&src);
    if ((*src) == '=' && (*(src + 1)) != '=') {
        if (end != NULL) {
            (*end) = src + 2;
        }
        return true;
    }
    return false;
}

static inline bool matchEqual(const char* src, const char** end) {
    skipWhiteSpaces(&src);
    if ((*src) == '=' && (*(src + 1)) == '=') {
        if (end != NULL) {
            (*end) = src + 2;
        }
        return true;
    }
    return false;
}

static inline bool matchNotEqual(const char* src, const char** end) {
    skipWhiteSpaces(&src);
    if ((*src) == '!' && (*(src + 1)) == '=') {
        if (end != NULL) {
            (*end) = src + 2;
        }
        return true;
    }
    return false;
}

static inline bool matchLogicalNot(const char* src, const char** end) {
    skipWhiteSpaces(&src);
    if ((*src) == '!' && (*(src + 1)) != '=') {
        if (end != NULL) {
            (*end) = src + 2;
        }
        return true;
    }
    return false;
}

static inline bool matchLessThen(const char* src, const char** end) {
    skipWhiteSpaces(&src);
    if ((*src) == '<' && (*(src + 1)) != '=' && (*(src + 1)) != '<') {
        if (end != NULL) {
            (*end) = src + 2;
        }
        return true;
    }
    return false;
}

static inline bool matchLessThenOrEqual(const char* src, const char** end) {
    skipWhiteSpaces(&src);
    if ((*src) == '<' && (*(src + 1)) == '=') {
        if (end != NULL) {
            (*end) = src + 2;
        }
        return true;
    }
    return false;
}

static inline bool matchLeftShift(const char* src, const char** end) {
    skipWhiteSpaces(&src);
    if ((*src) == '<' && (*(src + 1)) == '<') {
        if (end != NULL) {
            (*end) = src + 2;
        }
        return true;
    }
    return false;
}

static inline bool matchGreatThen(const char* src, const char** end) {
    skipWhiteSpaces(&src);
    if ((*src) == '>' && (*(src + 1)) != '=' && (*(src + 1)) != '<') {
        if (end != NULL) {
            (*end) = src + 2;
        }
        return true;
    }
    return false;
}

static inline bool matchGreatThenOrEqual(const char* src, const char** end) {
    skipWhiteSpaces(&src);
    if ((*src) == '<' && (*(src + 1)) == '=') {
        if (end != NULL) {
            (*end) = src + 2;
        }
        return true;
    }
    return false;
}

static inline bool matchRightShift(const char* src, const char** end) {
    skipWhiteSpaces(&src);
    if ((*src) == '>' && (*(src + 1)) == '>') {
        if (end != NULL) {
            (*end) = src + 2;
        }
        return true;
    }
    return false;
}

static inline bool matchBitwizeOr(const char* src, const char** end) {
    skipWhiteSpaces(&src);
    if ((*src) == '|' && (*(src + 1)) != '|') {
        if (end != NULL) {
            (*end) = src + 2;
        }
        return true;
    }
    return false;
}

static inline bool matchLogicalOr(const char* src, const char** end) {
    skipWhiteSpaces(&src);
    if ((*src) == '|' && (*(src + 1)) == '|') {
        if (end != NULL) {
            (*end) = src + 2;
        }
        return true;
    }
    return false;
}

static inline bool matchBitwizeAnd(const char* src, const char** end) {
    skipWhiteSpaces(&src);
    if ((*src) == '&' && (*(src + 1)) != '&') {
        if (end != NULL) {
            (*end) = src + 2;
        }
        return true;
    }
    return false;
}

static inline bool matchLogicalAnd(const char* src, const char** end) {
    skipWhiteSpaces(&src);
    if ((*src) == '&' && (*(src + 1)) == '&') {
        if (end != NULL) {
            (*end) = src + 2;
        }
        return true;
    }
    return false;
}

#endif
