#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>
#include <string.h>

#include "string.h"

struct Token {
    char* file_name;
    size_t line_number;
    // pointer to orginal string.
    struct String row_string;
    enum {
        TOKEN_NONE,

        TOKEN_UPPER_NAME,               // [A-Z][a-zA-Z0-9]*
        TOKEN_LOWER_NAME,               // [a-z][a-zA-Z0-9]*
        TOKEN_DOT_NAME,                 // .[a-zA-Z0-9]+
        TOKEN_AT_NAME,                  // @[a-zA-Z0-9]+
        TOKEN_STRING,                   // "[^"]*"

        TOKEN_KEYWORD_IMPORT,           // import
        TOKEN_KEYWORD_AS,               // as
        TOKEN_KEYWORD_TYPE,             // type
        TOKEN_KEYWORD_ENUM,             // enum
        TOKEN_KEYWORD_UNION,            // union
        TOKEN_KEYWORD_EXPORT,           // export
        TOKEN_KEYWORD_EXTERNAL,         // external
        TOKEN_KEYWORD_VAR,              // var
        TOKEN_KEYWORD_CONST,            // const
        TOKEN_KEYWORD_REF,              // ref
        TOKEN_KEYWORD_FUNC,             // func
        TOKEN_KEYWORD_CFUNC,            // cfunc
        TOKEN_KEYWORD_IF,               // if
        TOKEN_KEYWORD_ELSE,             // else
        TOKEN_KEYWORD_SWITH,            // swith
        TOKEN_KEYWORD_CASE,             // case
        TOKEN_KEYWORD_DEFAULT,          // default
        TOKEN_KEYWORD_LOOP,             // loop
        TOKEN_KEYWORD_BREAK,            // break
        TOKEN_KEYWORD_CONTINUE,         // continue
        TOKEN_KEYWORD_DO,               // do
        TOKEN_KEYWORD_WHILE,            // while
        TOKEN_KEYWORD_FOR,              // for
        TOKEN_KEYWORD_FOREATCH,         // foreatch
        TOKEN_KEYWORD_REPEAD,           // repead
        TOKEN_KEYWORD_RETURN,           // return
        TOKEN_KEYWORD_TEST,             // test

        TOKEN_FLOAT,
        TOKEN_INT,

        TOKEN_PARENTHESES_OPEN,         // (
        TOKEN_PARENTHESES_CLOSE,        // )
        TOKEN_BRACES_OPEN,              // {
        TOKEN_BRACES_CLOSE,             // }
        TOKEN_BRACKETS_OPEN,            // [
        TOKEN_BRACKETS_CLOSE,           // ]

        TOKEN_SEMICOLON,                // ;
        TOKEN_COLON,                    // :
        TOKEN_COMMA,                    // ,

        TOKEN_LESS_THEN,                // <
        TOKEN_GREATER_THEN,             // >
        TOKEN_LESS_OR_EQUAL_THEN,       // <=
        TOKEN_GREATER_OR_EQUAL_THEN,    // >=
        TOKEN_EQUAL,                    // ==
        TOKEN_NOT_EQUAL,                // !=

        TOKEN_LOGICAL_NOT,              // !
        TOKEN_LOGICAL_OR,               // ||
        TOKEN_LOGICAL_AND,              // &&

        TOKEN_SHIFT_LEFT,               // <<
        TOKEN_SHIFT_RIGHT,              // >>

        TOKEN_BITWIZE_NOT,              // ~
        TOKEN_BITWIZE_OR,               // |
        TOKEN_BITWIZE_AND,              // &

        TOKEN_PLUS,                     // +
        TOKEN_MINUS,                    // -
        TOKEN_MULTIPLY,                 // *
        TOKEN_DIVIDE,                   // /
        TOKEN_MODULO,                   // %

        TOKEN_ASIGN,                    // =

        TOKEN_ASIGN_SHIFT_LEFT,         // <<=
        TOKEN_ASIGN_SHIFT_RIGHT,        // >>=

        TOKEN_ASIGN_BITWIZE_NOT,        // ~=
        TOKEN_ASIGN_BITWIZE_OR,         // |=
        TOKEN_ASIGN_BITWIZE_AND,        // &=

        TOKEN_ASIGN_PLUS,               // +=
        TOKEN_ASIGN_MINUS,              // -=
        TOKEN_ASIGN_MULTIPLY,           // *=
        TOKEN_ASIGN_DIVIDE,             // /=
        TOKEN_ASIGN_MODULO,             // %=
    } value_type;
    union {
        struct String value_string;
        long double   value_float;
        uint64_t      value_int;
    };
};

struct Tokens {
    struct Token   token;
    struct Tokens* next;
};

struct Tokens* getTokens(const char* src, const char* file_name);
void freeTokens(struct Tokens* tokens);
void printToken(struct Token* token);
void printTokens(struct Tokens* tokens);

#endif
