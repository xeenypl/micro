#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "memory.h"

size_t current_line_number;
const char* current_file;

static inline void errorIlligalCharacter(char character) {
    fprintf(
        stderr,
        "Parsing Error %s:%ld: illigal character '%c'\n",
        current_file,
        current_line_number,
        character
    );
    exit(EXIT_FAILURE);
}

static inline void errorIlligalEscape(char character) {
    fprintf(
        stderr,
        "Parsing Error %s:%ld: illigal escape character '%c'\n",
        current_file,
        current_line_number,
        character
    );
}

static inline void errorIlligalNewLine(void) {
    fprintf(
        stderr,
        "Parsing Error %s:%ld: illigal new line\n",
        current_file,
        current_line_number
    );
    exit(EXIT_FAILURE);
}

static inline void errorIlligalName(void) {
    fprintf(
        stderr,
        "Parsing Error %s:%ld: illigal name\n",
        current_file,
        current_line_number
    );
    exit(EXIT_FAILURE);
}

static inline void skipSpaces(const char** src) {
    while (isspace(**src)) {
        if ((**src) == '\n') {
            current_line_number++;
        }
        (*src)++;
    }
}

static inline void skipOneLineComment(const char** src) {
    current_line_number++;
    while ((**src) != '\n') {
        (*src)++;
    }
    (*src)++;
}

static inline void skipMultiLineComment(const char** src) {
    while (!((**src) == '/' && (*((*src) - 1)) == '*')) {
        if ((**src) == '\n') {
            current_line_number++;
        }
        (*src)++;
    }
    (*src)++;
}

void skipWhiteSpaces(const char** src) {
    while (true) {
        if (isspace(**src)) {
            skipSpaces(src);
            continue;
        }

        if ((**src) == '/' && (*((*src) + 1)) == '/') {
            skipOneLineComment(src);
            continue;
        }

        if ((**src) == '/' && (*((*src) + 1)) == '*') {
            skipMultiLineComment(src);
            continue;
        }

        break;
    }
}

static char parseHexDigit(char c) {
    if ('0' <= c && c <= '9') {
        return c - '0';
    } else if ('A' <= c && c <= 'F') {
        return c - 'A' + 10;
    } else if ('a' <= c && c <= 'f') {
        return c - 'a' + 10;
    }
    return 0;
}

static char parseHex(const char* str) {
    char res = 0;
    res = res | parseHexDigit(*str);
    res = res << 4;
    res = res | parseHexDigit(*(str + 1));
    return res;
}

static struct String escape(const char* str, size_t length) {
    char* res = memoryAlloc(length + 1);
    size_t res_length = 0;
    for (size_t i = 0; i < length; i++) {
        if (str[i] == '\\') {
            switch (str[++i]) {
            case '\\':
                res[res_length++] = '\\';
                break;
            case '"':
                res[res_length++] = '"';
                break;
            case '\'':
                res[res_length++] = '\'';
                break;
            case 'b':
                res[res_length++] = '\b';
                break;
            case 'a':
                res[res_length++] = '\a';
                break;
            case 'e':
                res[res_length++] = 0x1b;
                break;
            case 'f':
                res[res_length++] = '\f';
                break;
            case 'n':
                res[res_length++] = '\n';
                break;
            case 'r':
                res[res_length++] = '\r';
                break;
            case 't':
                res[res_length++] = '\t';
                break;
            case 'v':
                res[res_length++] = '\v';
                break;
            case 'x':
                i++;
                res[res_length++] = parseHex(str + i);
                i++;
                break;
            default:
                errorIlligalEscape(str[i]);
                res[res_length++] = '/';
                res[res_length++] = str[i];
            }
        } else if (str[i] == '\n') {
            errorIlligalNewLine(); 
            break;
        } else {
            res[res_length++] = str[i];
        }
    }
    return (struct String) {
        .string = strndup(res, res_length),
        .length = res_length
    };
}

// TODO: proper implement 
bool matchUint(const char* src, const char** end, uint64_t* result) {
    (*result) = strtoull(src, (char**)end, 0);
    return src != (*end);
}

bool matchFloat(const char*  src, const char** end, long double* result) {
    (*result) = strtold(src, (char**)end);
    return src != (*end);
}

bool matchString(const char* src, const char** end, struct String* result) {
    skipWhiteSpaces(&src);
    if ((*src) == '"') {
        size_t length = 1;;
        while (!(((src[length] == '"')
               && (src[length - 1] != '\\'))
              || ((src[length] == '"')
               && (src[length - 1] == '\\')
               && (src[length - 2] == '\\')))) {
            length++;
        }
        length++;
        (*end) = src + length;
        (*result) = escape(src + 1, length - 2);
        return true;
    }
    return false;
}

bool matchKeyword(const char* src, const char** end, const char*  str) {
    skipWhiteSpaces(&src);
    size_t length = strlen(str);
    if (strncmp(src, str, length) == 0 && !isalnum(src[length])) {
        if (end != NULL) {
            (*end) = src + length;
        }
        return true;
    }
    return false;
} 

bool matchUpperName(const char* src, const char** end, struct String* result) {
    skipWhiteSpaces(&src);
    if (isupper(*src)) {
        size_t length = 1;
        while (isalnum(src[length])) {
            length++;
        }
        if (end != NULL) {
            (*end) = src + length;
        }
        if (result != NULL) {
            (*result) = newStringL(src, length);
        }
        return true;
    }
    return false;
}

bool matchLowerName(const char* src, const char** end, struct String* result) {
    skipWhiteSpaces(&src);
    if (islower(*src)) {
        size_t length = 1;
        while (isalnum(src[length])) {
            length++;
        }
        if (end != NULL) {
            (*end) = src + length;
        }
        if (result != NULL) {
            (*result) = newStringL(src, length);
        }
        return true;
    }
    return false;
}

bool matchDotName(const char* src, const char** end, struct String* result) {
    skipWhiteSpaces(&src);
    if ((*src) == '.') {
        size_t length = 1;
        while (isalnum(src[length])) {
            length++;
        }
        if (end != NULL) {
            (*end) = src + length;
        }
        if (result != NULL) {
            (*result) = newStringL(src, length);
        }
        return true;
    }
    return false;
}

bool matchAtName(const char* src, const char** end, struct String* result) {
    skipWhiteSpaces(&src);
    if ((*src) == '@') {
        size_t length = 1;
        while (isalnum(src[length])) {
            length++;
        }
        if (end != NULL) {
            (*end) = src + length;
        }
        if (result != NULL) {
            (*result) = newStringL(src, length);
        }
        return true;
    }
    return false;
}
