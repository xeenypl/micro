#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "memory.h"

static inline void errorEndOfToken(void) {
    fprintf(
        stderr,
        "Syntax error: end of tokens\n"
    );
    exit(EXIT_FAILURE);
}

static inline void errorUnexpextedToken(struct Token token) {
    fprintf(
        stderr,
        "Syntax error %s:%ld unexpected token %s\n",
        token.file_name,
        token.line_number,
        tokenType(token.type)
    );
    exit(EXIT_FAILURE);
}

static inline void nextToken(struct TokenList** stream) {
    if ((*stream) -> next -> token.type == TOKEN_NONE) {
        errorEndOfToken();
    }
    (*stream) = (*stream) -> next;
}

static inline bool tokenIs(struct TokenList** stream, enum TokenType type) {
    return (*stream) -> token.type == type;
}

static inline bool matchToken(struct TokenList** stream, enum TokenType type) {
    if ((*stream) -> token.type == type) {
        nextToken(stream);
        return true;
    }
    return false;
}

static inline struct Token* eatToken(struct TokenList** stream) {
    if ((*stream) -> next == NULL) {
        errorEndOfToken();
    }
    struct TokenList *now = (*stream);
    (*stream) = (*stream) -> next;
    return &(now -> token);
}

static inline struct Token* acceptToken(
    struct TokenList** stream, 
    enum TokenType type
) {
    if ((*stream) -> token.type == type) {
        return eatToken(stream);
    }
    errorUnexpextedToken((*stream) -> token);
    return NULL;
}

static struct Path* parsePathTail(struct TokenList** stream) {
    struct Path* res = memoryAlloc(sizeof(struct Path));
    struct Path* now = res;
    while (tokenIs(stream, TOKEN_DOT_NAME)) {
        struct Token* tok = eatToken(stream);
        appendPath(&now, tok -> value_string);
    }
    return res;
}

static struct Path* parsePath(struct TokenList** stream) {
    if (tokenIs(stream, TOKEN_UPPER_NAME)
     || tokenIs(stream, TOKEN_LOWER_NAME)) {
        struct Path* res = memoryAlloc(sizeof(struct Path));
        struct Token* tok = eatToken(stream);
        res -> name = tok -> value_string;
        if (tokenIs(stream, TOKEN_DOT_NAME)) {
            res -> next = parsePathTail(stream);
        }
        return res;
    }
    errorUnexpextedToken((*stream) -> token);
    return NULL;
}

static struct Import parseImport(struct TokenList** stream) {
    acceptToken(stream, TOKEN_KEYWORD_IMPORT);
    struct Path* res_path;
    if (tokenIs(stream, TOKEN_UPPER_NAME)
     || tokenIs(stream, TOKEN_LOWER_NAME)) {
        res_path = parsePath(stream);
    } else if (tokenIs(stream, TOKEN_DOT_NAME)) {
        res_path = parsePathTail(stream);
    } else {
        errorUnexpextedToken((*stream) -> token);
    }
    bool res_is_rename = false;
    struct String res_as;
    if (matchToken(stream, TOKEN_KEYWORD_AS)) {
        struct Token* tok = acceptToken(stream, TOKEN_LOWER_NAME);
        res_is_rename = true;
        res_as = tok -> value_string;
    }
    acceptToken(stream, TOKEN_SEMICOLON);
    return (struct Import) {
        .is_rename = res_is_rename,
        .as        = res_as,
        .path      = res_path
    };
}

static struct TypeHeader parseTypeHeader(struct TokenList** stream) {
    struct Token* name = acceptToken(stream, TOKEN_UPPER_NAME);
    struct TypeParams* params = NULL;
    if (matchToken(stream, TOKEN_CHEVRONS_OPEN)) {
        params = memoryAlloc(sizeof(struct TypeParams));
        struct TypeParams* now = params;
        do {
            struct Token* name = acceptToken(stream, TOKEN_UPPER_NAME);
            appendTypeParams(&now, name -> value_string);
        } while (matchToken(stream, TOKEN_COMMA));
        acceptToken(stream, TOKEN_CHEVRONS_CLOSE);
    }
    return (struct TypeHeader) {
        .name   = name -> value_string,
        .params = params
    };
}

static struct Type parseType(struct TokenList** stream) {
    bool is_ref = matchToken(stream, TOKEN_KEYWORD_REF);
    struct Token* name = acceptToken(stream, TOKEN_UPPER_NAME);
    struct TypeArgs* args = NULL;
    if (matchToken(stream, TOKEN_CHEVRONS_OPEN)) {
        args = memoryAlloc(sizeof(struct TypeParams));
        struct TypeArgs* now = args;
        do {
            struct Type arg = parseType(stream);
            appendTypeArgs(&now, arg);
        } while (matchToken(stream, TOKEN_COMMA));
        acceptToken(stream, TOKEN_CHEVRONS_CLOSE);
    }
    return (struct Type) {
        .is_ref = is_ref,
        .name   = name -> value_string,
        .args   = args
    };
}

static struct TypeFild parseTypeFild(struct TokenList** stream) {
    struct Token* name = acceptToken(stream, TOKEN_LOWER_NAME);
    acceptToken(stream, TOKEN_COLON);
    struct Type type = parseType(stream);
    acceptToken(stream, TOKEN_SEMICOLON);
    return (struct TypeFild) {
        .name = name -> value_string, 
        .type = type
    };
}

static struct TypeFildList* paresTypeFildList(struct TokenList** stream) {
    acceptToken(stream, TOKEN_BRACES_OPEN);
    struct TypeFildList* res = memoryAlloc(sizeof(struct TypeFildList));
    struct TypeFildList* now = res;
    do {
        struct TypeFild fild = parseTypeFild(stream);
        appendTypeFildList(&now, fild);
    } while (!matchToken(stream, TOKEN_BRACES_CLOSE));
    return res;
}

static struct EnumFildList* parseEnumFildList(struct TokenList** stream) {
    acceptToken(stream, TOKEN_KEYWORD_ENUM);
    acceptToken(stream, TOKEN_BRACES_OPEN);
    struct EnumFildList* res = memoryAlloc(sizeof(struct EnumFildList));
    struct EnumFildList* now = res;
    do {
        struct Token* name = acceptToken(stream, TOKEN_LOWER_NAME);
        if (matchToken(stream, TOKEN_COLON)) {
            struct Type type = parseType(stream);
            acceptToken(stream, TOKEN_SEMICOLON);
            appendEnumFildTyped(
                &now, 
                (struct TypeFild) {
                    .name = name -> value_string, 
                    .type = type
                }
            );
        } else {
            acceptToken(stream, TOKEN_SEMICOLON);
            appendEnumFildUntyped(&now, name -> value_string);
        }
    } while (!matchToken(stream, TOKEN_BRACES_CLOSE));
    return res;
}

static struct TypeFildList* parseUnion(struct TokenList** stream) {
    acceptToken(stream, TOKEN_KEYWORD_UNION);
    return paresTypeFildList(stream);
}

static struct TypeDeclFild parseTypeDeclFild(struct TokenList** stream) {
    struct Token* name = acceptToken(stream, TOKEN_LOWER_NAME);
    acceptToken(stream, TOKEN_COLON);
    if (tokenIs(stream, TOKEN_KEYWORD_ENUM)) {
        struct EnumFildList* res = parseEnumFildList(stream);
        acceptToken(stream, TOKEN_SEMICOLON);
        return (struct TypeDeclFild) {
            .name  = name -> value_string,
            .type  = TYPE_FILD_ENUM,
            ._enum = res
        };
    }
    if (tokenIs(stream, TOKEN_KEYWORD_UNION)) {
        struct TypeFildList* res = parseUnion(stream);
        acceptToken(stream, TOKEN_SEMICOLON);
        return (struct TypeDeclFild) {
            .name   = name -> value_string,
            .type   = TYPE_FILD_UNION,
            ._union = res
        };
    }
    struct Type type = parseType(stream);
    acceptToken(stream, TOKEN_SEMICOLON);
    return (struct TypeDeclFild) {
        .name  = name -> value_string,
        .type  = TYPE_FILD_ENUM,
        ._type = type
    };
}

static struct TypeDeclFildList* paresTypeDeclFildList(
    struct TokenList** stream
) {
    acceptToken(stream, TOKEN_BRACES_OPEN);
    struct TypeDeclFildList* res = memoryAlloc(sizeof(struct TypeDeclFildList));
    struct TypeDeclFildList* now = res;
    do {
        struct TypeDeclFild fild = parseTypeDeclFild(stream);
        appendTypeDeclFildList(&now, fild);
    } while (!matchToken(stream, TOKEN_BRACES_CLOSE));
    return res;
}

static struct TypeDecl parseTypeDecl(struct TokenList** stream, bool exported) {
    acceptToken(stream, TOKEN_KEYWORD_TYPE);
    struct TypeHeader header = parseTypeHeader(stream);

    if (matchToken(stream, TOKEN_ASIGN)) {
        struct Type res = parseType(stream);
        acceptToken(stream, TOKEN_SEMICOLON);
        return (struct TypeDecl) {
            .is_exported = exported,
            .header       = header,
            .type        = TYPE_TYPE,
            ._type       = res
        };
    } 

    if (tokenIs(stream, TOKEN_KEYWORD_ENUM)) {
        struct EnumFildList* res = parseEnumFildList(stream);
        acceptToken(stream, TOKEN_SEMICOLON);
        return (struct TypeDecl) {
            .is_exported = exported,
            .header       = header,
            .type        = TYPE_ENUM,
            ._enum       = res
        };
    }

    if (tokenIs(stream, TOKEN_BRACES_OPEN)) {
        struct TypeDeclFildList* res =  paresTypeDeclFildList(stream);
        acceptToken(stream, TOKEN_SEMICOLON);
        return (struct TypeDecl) {
            .is_exported = exported,
            .header      = header,
            .type        = TYPE_STRUCT,
            ._struct     = res
        };
    }

    errorUnexpextedToken((*stream) -> token);
    return (struct TypeDecl) { 0 };
}

struct AST* parse(struct TokenList* stream) {
    struct AST* res = memoryAlloc(sizeof(struct AST));
    struct AST* now = res;
    while (stream -> token.type != TOKEN_NONE) {
        switch (stream -> token.type) {
        case TOKEN_KEYWORD_EXPORT:
            nextToken(&stream);
            switch (stream -> token.type) {
            case TOKEN_KEYWORD_TYPE:
                addType(&now, parseTypeDecl(&stream, true));
                break;
            case TOKEN_KEYWORD_FUNC:
                break;
            default:
                errorUnexpextedToken(stream -> token);
            }
            break;
        case TOKEN_KEYWORD_EXTERNAL:
            nextToken(&stream);
            switch (stream -> token.type) {
            case TOKEN_KEYWORD_FUNC:
                break;
            default:
                errorUnexpextedToken(stream -> token);
            }
            break;
        case TOKEN_KEYWORD_IMPORT:
            addImport(&now, parseImport(&stream));
            break;
        case TOKEN_KEYWORD_TYPE:
            addType(&now, parseTypeDecl(&stream, false));
            break;
        case TOKEN_KEYWORD_FUNC:
            break;
        case TOKEN_KEYWORD_CFUNC:
            break;
        case TOKEN_KEYWORD_TEST:
            break;
        default:
            errorUnexpextedToken(stream -> token);
        }
    }
    return res;
}
