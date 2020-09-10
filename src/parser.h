#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

struct AST* parse(struct TokenList* tokens);

#endif
