#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>
#include <string.h>

#include "string.h"

enum TokenType {
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
    TOKEN_KEYWORD_DEREF,            // deref
    TOKEN_KEYWORD_FUNC,             // func
    TOKEN_KEYWORD_CFUNC,            // cfunc
    TOKEN_KEYWORD_IF,               // if
    TOKEN_KEYWORD_ELSE,             // else
    TOKEN_KEYWORD_SWITCH,           // switch
    TOKEN_KEYWORD_CASE,             // case
    TOKEN_KEYWORD_DEFAULT,          // default
    TOKEN_KEYWORD_LOOP,             // loop
    TOKEN_KEYWORD_BREAK,            // break
    TOKEN_KEYWORD_CONTINUE,         // continue
    TOKEN_KEYWORD_DO,               // do
    TOKEN_KEYWORD_WHILE,            // while
    TOKEN_KEYWORD_FOR,              // for
    TOKEN_KEYWORD_IN,               // in
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
    TOKEN_CHEVRONS_OPEN,            // <
    TOKEN_CHEVRONS_CLOSE,           // >
    
    TOKEN_SEMICOLON,                // ;
    TOKEN_COLON,                    // :
    TOKEN_COMMA,                    // ,
    
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
};

struct Token {
    char*          file_name;
    size_t         line_number;
    // pointer to orginal string.
    struct String  row_string;
    enum TokenType type;
    union {
        struct String value_string;
        long double   value_float;
        uint64_t      value_int;
    };
};

struct TokenList {
    struct Token      token;
    struct TokenList* next;
};

static inline const char* tokenType(enum TokenType type) {
    switch (type) {
    case TOKEN_NONE:
        return "NONE";
    
    case TOKEN_UPPER_NAME:
        return "NAME";
    case TOKEN_LOWER_NAME:
        return "NAME";
    case TOKEN_DOT_NAME:
        return "NAME";
    case TOKEN_AT_NAME:
        return "NAME";
    case TOKEN_STRING:
        return "STRING";
    
    case TOKEN_KEYWORD_IMPORT:
        return "KEYWORD import";
    case TOKEN_KEYWORD_AS:
        return "KEYWORD as";
    case TOKEN_KEYWORD_TYPE:
        return "KEYWORD type";
    case TOKEN_KEYWORD_ENUM:
        return "KEYWORD enum";
    case TOKEN_KEYWORD_UNION:
        return "KEYWORD union";
    case TOKEN_KEYWORD_EXPORT:
        return "KEYWORD export";
    case TOKEN_KEYWORD_EXTERNAL:
        return "KEYWORD external";
    case TOKEN_KEYWORD_VAR:
        return "KEYWORD var";
    case TOKEN_KEYWORD_CONST:
        return "KEYWORD const";
    case TOKEN_KEYWORD_REF:
        return "KEYWORD ref";
    case TOKEN_KEYWORD_DEREF:
        return "KEYWORD deref";
    case TOKEN_KEYWORD_FUNC:
        return "KEYWORD func";
    case TOKEN_KEYWORD_CFUNC:
        return "KEYWORD cfunc";
    case TOKEN_KEYWORD_IF:
        return "KEYWORD if";
    case TOKEN_KEYWORD_ELSE:
        return "KEYWORD else";
    case TOKEN_KEYWORD_SWITCH:
        return "KEYWORD switch";
    case TOKEN_KEYWORD_CASE:
        return "KEYWORD case";
    case TOKEN_KEYWORD_DEFAULT:
        return "KEYWORD default";
    case TOKEN_KEYWORD_LOOP:
        return "KEYWORD loop";
    case TOKEN_KEYWORD_BREAK:
        return "KEYWORD break";
    case TOKEN_KEYWORD_CONTINUE:
        return "KEYWORD continue";
    case TOKEN_KEYWORD_DO:
        return "KEYWORD do";
    case TOKEN_KEYWORD_WHILE:
        return "KEYWORD while";
    case TOKEN_KEYWORD_FOR:
        return "KEYWORD for";
    case TOKEN_KEYWORD_IN:
        return "KEYWORD in";
    case TOKEN_KEYWORD_REPEAD:
        return "KEYWORD repead";
    case TOKEN_KEYWORD_RETURN:
        return "KEYWORD return";
    case TOKEN_KEYWORD_TEST:
        return "KEYWORD test";
    
    case TOKEN_FLOAT:
        return "FLOAT";
    case TOKEN_INT:
        return "INT";
    
    case TOKEN_PARENTHESES_OPEN:
        return "(";
    case TOKEN_PARENTHESES_CLOSE:
        return ")";
    case TOKEN_BRACES_OPEN:
        return "{";
    case TOKEN_BRACES_CLOSE:
        return "}";
    case TOKEN_BRACKETS_OPEN:
        return "[";
    case TOKEN_BRACKETS_CLOSE:
        return "]";
    case TOKEN_CHEVRONS_OPEN:
        return "<";
    case TOKEN_CHEVRONS_CLOSE:
        return ">";
    
    case TOKEN_SEMICOLON:
        return ";";
    case TOKEN_COLON:
        return ":";
    case TOKEN_COMMA:
        return ",";
    
    case TOKEN_LESS_OR_EQUAL_THEN:
        return "<=";
    case TOKEN_GREATER_OR_EQUAL_THEN:
        return ">=";
    case TOKEN_EQUAL:
        return "==";
    case TOKEN_NOT_EQUAL:
        return "!=";
    
    case TOKEN_LOGICAL_NOT:
        return "!";
    case TOKEN_LOGICAL_OR:
        return "||";
    case TOKEN_LOGICAL_AND:
        return "&&";
    
    case TOKEN_SHIFT_LEFT:
        return "<<";
    case TOKEN_SHIFT_RIGHT:
        return ">>";
    
    case TOKEN_BITWIZE_NOT:
        return "~";
    case TOKEN_BITWIZE_OR:
        return "|";
    case TOKEN_BITWIZE_AND:
        return "&";
    
    case TOKEN_PLUS:
        return "+";
    case TOKEN_MINUS:
        return "-";
    case TOKEN_MULTIPLY:
        return "*";
    case TOKEN_DIVIDE:
        return "/";
    case TOKEN_MODULO:
        return "%";
    
    case TOKEN_ASIGN:
        return "=";
    
    case TOKEN_ASIGN_SHIFT_LEFT:
        return "<<=";
    case TOKEN_ASIGN_SHIFT_RIGHT:
        return ">>=";
    
    case TOKEN_ASIGN_BITWIZE_NOT:
        return "~=";
    case TOKEN_ASIGN_BITWIZE_OR:
        return "|=";
    case TOKEN_ASIGN_BITWIZE_AND:
        return "&=";
    
    case TOKEN_ASIGN_PLUS:
        return "+=";
    case TOKEN_ASIGN_MINUS:
        return "-=";
    case TOKEN_ASIGN_MULTIPLY:
        return "*=";
    case TOKEN_ASIGN_DIVIDE:
        return "/=";
    case TOKEN_ASIGN_MODULO:
        return "%=";
    }
    return "idk";
}

struct TokenList* getTokens(const char* src, const char* file_name);
void freeTokens(struct TokenList* tokens);
void printToken(struct Token* token);
void printTokens(struct TokenList* tokens);

#endif
