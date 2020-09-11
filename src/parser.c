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

static inline void errorUnexpextedToken(const char* stream) {
    fprintf(
        stderr,
        "Syntax error %s:%ld\n",
        // TODO: make better error mesage
        current_file,
        current_line_number
    );
    exit(EXIT_FAILURE);
}

static inline void assertSyntax(bool test, const char* stream) {
    if (!test) {
        errorUnexpextedToken(stream);
    }
}

static struct Path* parsePathTail(const char** stream) {
    struct Path* res = memoryAlloc(sizeof(struct Path));
    struct Path* now = res;
    struct String name;
    while (matchDotName(*stream, stream, &name)) {
        appendPath(&now, name);
    }
    return res;
}

static struct Path* parsePath(const char** stream) {
    struct Path* res = memoryAlloc(sizeof(struct Path));
    struct String name;
    if (matchUpperName(*stream, stream, &name)
     || matchLowerName(*stream, stream, &name)) {
        res -> name = name;
        if (matchDotName(*stream, NULL, NULL)) {
            res -> next = parsePathTail(stream);
        }
        return res;
    }
    errorUnexpextedToken(*stream);
    return NULL;
}

static struct Import parseImport(const char** stream) {
    struct Path* res_path;
    if (matchUpperName(*stream, NULL, NULL)
     || matchLowerName(*stream, NULL, NULL)) {
        res_path = parsePath(stream);
    } else if (matchDotName(*stream, NULL, NULL)) {
        res_path = parsePathTail(stream);
    } else {
        errorUnexpextedToken(*stream);
    }
    bool res_is_rename = false;
    struct String res_as;
    if (matchKeyword(*stream, stream, "as")) {
        assertSyntax(matchLowerName(*stream, stream, &res_as), *stream);
        res_is_rename = true;
    }
    assertSyntax(matchChar(*stream, stream, ';'), *stream);
    return (struct Import) {
        .is_rename = res_is_rename,
        .as        = res_as,
        .path      = res_path
    };
}

static struct TypeHeader parseTypeHeader(const char** stream) {
    struct String name;
    assertSyntax(matchUpperName(*stream, stream, &name), *stream);
    struct TypeParams* params = NULL;
    if (matchChar(*stream, stream, '<')) {
        params = memoryAlloc(sizeof(struct TypeParams));
        struct TypeParams* now = params;
        do {
            struct String param;
            assertSyntax(matchUpperName(*stream, stream, &param), *stream);
            appendTypeParams(&now, param);
        } while (matchChar(*stream, stream, ','));
        assertSyntax(matchChar(*stream, stream, '>'), *stream);
    }
    return (struct TypeHeader) {
        .name   = name,
        .params = params
    };
}

static struct Type parseType(const char** stream) {
    bool is_ref = matchKeyword(*stream, stream, "ref");
    struct String name;
    assertSyntax(matchUpperName(*stream, stream, &name), *stream);
    struct TypeArgs* args = NULL;
    if (matchChar(*stream, stream, '<')) {
        args = memoryAlloc(sizeof(struct TypeParams));
        struct TypeArgs* now = args;
        do {
            struct Type arg = parseType(stream);
            appendTypeArgs(&now, arg);
        } while (matchChar(*stream, stream, ','));
        assertSyntax(matchChar(*stream, stream, '>'), *stream);
    }
    return (struct Type) {
        .is_ref = is_ref,
        .name   = name,
        .args   = args
    };
}

static struct TypeFild parseTypeFild(const char** stream) {
    struct String name;
    assertSyntax(matchLowerName(*stream, stream, &name), *stream);
    assertSyntax(matchChar(*stream, stream, ':'), *stream);
    struct Type type = parseType(stream);
    assertSyntax(matchChar(*stream, stream, ';'), *stream);
    return (struct TypeFild) {
        .name = name, 
        .type = type
    };
}

static struct TypeFildList* paresTypeFildList(const char** stream) {
    assertSyntax(matchChar(*stream, stream, '{'), *stream);
    struct TypeFildList* res = memoryAlloc(sizeof(struct TypeFildList));
    struct TypeFildList* now = res;
    do {
        struct TypeFild fild = parseTypeFild(stream);
        appendTypeFildList(&now, fild);
    } while (!matchChar(*stream, stream, '}'));
    return res;
}

static struct EnumFildList* parseEnumFildList(const char** stream) {
    assertSyntax(matchChar(*stream, stream, '{'), *stream);
    struct EnumFildList* res = memoryAlloc(sizeof(struct EnumFildList));
    struct EnumFildList* now = res;
    do {
        struct String name;
        assertSyntax(matchLowerName(*stream, stream, &name), *stream);
        if (matchChar(*stream, stream, ':')) {
            struct Type type = parseType(stream);
            appendEnumFildTyped(
                &now, 
                (struct TypeFild) {
                    .name = name, 
                    .type = type
                }
            );
        } else {
            appendEnumFildUntyped(&now, name);
        }
        assertSyntax(matchChar(*stream, stream, ';'), *stream);
    } while (!matchChar(*stream, stream, '}'));
    return res;
}

static struct TypeDecl parseTypeDecl(const char** stream, bool exported) {
    struct TypeHeader header = parseTypeHeader(stream);

    if (matchChar(*stream, stream, '=')) {
        struct Type res = parseType(stream);
        assertSyntax(matchChar(*stream, stream, ';'), *stream);
        return (struct TypeDecl) {
            .is_exported = exported,
            .header      = header,
            .type        = TYPE_TYPE,
            ._type       = res
        };
    } 

    if (matchKeyword(*stream, stream, "enum")) {
        struct EnumFildList* res = parseEnumFildList(stream);
        assertSyntax(matchChar(*stream, stream, ';'), *stream);
        return (struct TypeDecl) {
            .is_exported = exported,
            .header      = header,
            .type        = TYPE_ENUM,
            ._enum       = res
        };
    }

    if (matchKeyword(*stream, stream, "union")) {
        struct TypeFildList* res = paresTypeFildList(stream);
        assertSyntax(matchChar(*stream, stream, ';'), *stream);
        return (struct TypeDecl) {
            .is_exported = exported,
            .header      = header,
            .type        = TYPE_UNION,
            ._union      = res
        };
    }

    if (matchChar(*stream, NULL, '{')) {
        struct TypeFildList* res = paresTypeFildList(stream);
        assertSyntax(matchChar(*stream, stream, ';'), *stream);
        return (struct TypeDecl) {
            .is_exported = exported,
            .header      = header,
            .type        = TYPE_STRUCT,
            ._struct     = res
        };
    }

    errorUnexpextedToken(*stream);
    return (struct TypeDecl) { 0 };
}

struct AST* parse(const char* stream, const char* file_name) {
    current_file = file_name;
    current_line_number = 1;
    struct AST* res = memoryAlloc(sizeof(struct AST));
    struct AST* now = res;

    while ((*stream) != 0) {
        skipWhiteSpaces(&stream);
        if (matchKeyword(stream, &stream, "import")) {
            addImport(&now, parseImport(&stream));
            continue;
        }

        if (matchKeyword(stream, &stream, "type")) {
            addType(&now, parseTypeDecl(&stream, false));
            continue;
        }

        if (matchKeyword(stream, &stream, "export")) { 
            if (matchKeyword(stream, &stream, "type")) {
                addType(&now, parseTypeDecl(&stream, true));
                continue;
            }
        }
    }

    return res;
}
