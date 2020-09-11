#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

struct AST* parse(const char* src, const char* file_name);

#endif
