#include "ast.h"
#include "compile.h"
#include "file.h"
#include "lexer.h"
#include "memory.h"
#include "parser.h"

void compile(const char* path) {
    char* src = readFile(path);
    struct AST* ast = parse(src, path);
    printAST(stdout, ast);
    memoryFree(src);
}
