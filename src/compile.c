#include "compile.h"
#include "file.h"
#include "lexer.h"
#include "memory.h"

void compile(const char* path) {
    const char* src = readFile(path);
    struct Tokens* tokens = getTokens(src, path);
    printTokens(tokens);
    freeTokens(tokens);
    memoryFree((void*)src);
}
