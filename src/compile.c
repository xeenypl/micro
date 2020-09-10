#include "ast.h"
#include "compile.h"
#include "file.h"
#include "lexer.h"
#include "memory.h"
#include "parser.h"

void compile(const char* path) {
    char* src = readFile(path);
    struct TokenList* tokens = getTokens(src, path);
//    printTokens(tokens);
    struct AST* ast = parse(tokens);
    printAST(stdout, ast);
    freeTokens(tokens);
    memoryFree(src);
}
