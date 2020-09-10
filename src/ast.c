#include <stdio.h>

#include "ast.h"

static void printPath(FILE *stream, struct Path* path) {
    while (path != NULL) {
        fprintf(
            stream,
            "%.*s",
            (int) path -> name.length,
            path -> name.string
        );
        path = path -> next;
    }
}

static void printImport(FILE *stream, struct Import import) {
    fprintf(stream, "import ");
    printPath(stream, import.path);
    if (import.is_rename) {
        fprintf(
            stream,
            " as %.*s",
            (int) import.as.length,
            import.as.string
        );
    }
    fprintf(stream, ";\n");
}

static void printTypeParams(FILE *stream, struct TypeParams* params) {
    if (params != NULL) {
        fprintf(stream, "<");
        while (params != NULL) {
            fprintf(
                stream,
                "%.*s",
                (int) params -> name.length,
                params -> name.string
            );

            params = params -> next;
            if (params -> next == NULL) {
                break;
            }
            fprintf(stream, ", ");
        }
        fprintf(stream, ">");
    }
}

static void printTypeHeader(FILE *stream, struct TypeHeader header) {
    fprintf(
        stream,
        "%.*s ",
        (int) header.name.length,
        header.name.string
    );
    printTypeParams(stream, header.params);
}

static void printType(FILE *stream, struct Type type);
static void printTypeArgs(FILE *stream, struct TypeArgs* args) {
    if (args != NULL) {
        fprintf(stream, "(");
        while (args != NULL) {
            printType(
                stream,
                args -> type
            );

            args = args -> next;
            if (args -> next == NULL) {
                break;
            }
            fprintf(stream, ", ");
        }
        fprintf(stream, ")");
    }
}

static void printType(FILE *stream, struct Type type) {
    fprintf(
        stream,
        "%.*s",
        (int) type.name.length,
        type.name.string
    );
    printTypeArgs(stream, type.args);
}

static void printTypeDecl(FILE *stream, struct TypeDecl type) {
    fprintf(stream, "\n");
    if (type.is_exported) {
        printf("export ");
    }
    fprintf(stream, "type ");
    printTypeHeader(stream, type.header);
    switch (type.type) {
    case TYPE_TYPE:
        fprintf(stream, "= ");
        printType(stream, type._type);
        break;
    }
    

    fprintf(stream, ";\n");
}

void printAST(FILE *stream, struct AST* ast) {
    while (ast != NULL) {
        switch (ast -> type) {
        case AST_IMPORT:
            printImport(stream, ast -> ast_import);
            break;
        case AST_TYPE:
            printTypeDecl(stream, ast -> ast_type);
            break;
        }
        ast = ast -> next;
    }
}

