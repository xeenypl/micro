#include <stdbool.h>
// for: bool, true, false
#include <stddef.h>
// for: size_t
#include <stdio.h>
// for: printf, fprintf, stderr
#include <stdint.h>
// for: int64_t, uint64_t
#include <stdlib.h>
// for: exit, EXIT_FAILURE

#include "lexer.h"
#include "memory.h"

static size_t      current_line_number;
static const char* current_file;

static inline void errorIlligalCharacter(char character) {
    fprintf(
        stderr, 
        "Parsing Error %s:%ld: illigal character '%c'\n", 
        current_file,
        current_line_number,
        character
    );
    exit(EXIT_FAILURE);
}

static inline void errorIlligalEscape(char character) {
    fprintf(
        stderr, 
        "Parsing Error %s:%ld: illigal escape character '%c'\n", 
        current_file,
        current_line_number,
        character
    );
}

static inline void errorIlligalNewLine(void) {
    fprintf(
        stderr, 
        "Parsing Error %s:%ld: illigal new line\n", 
        current_file,
        current_line_number
    );
}

static inline void errorIlligalName(void) {
    fprintf(
        stderr, 
        "Parsing Error %s:%ld: illigal name\n", 
        current_file,
        current_line_number
    );
    exit(EXIT_FAILURE);
}

static inline bool isWhiteSpace(char c) {
    return (c == ' ') || (c == '\t') || (c == '\n');
}

static inline bool isUpperCase(char c) {
    return ('A' <= c) && (c <= 'Z');
}

static inline bool isLowerCase(char c) {
    return ('a' <= c) && (c <= 'z');
}

static inline bool isNumeric(char c) {
    return ('0' <= c) && (c <= '9');
}

static inline bool isHexDigit(char c) {
    return (('A' <= c) && (c <= 'H'))
        || (('a' <= c) && (c <= 'h'))
        || (('0' <= c) && (c <= '9'));
}

static inline bool isAlfaNumeric(char c) {
    return isUpperCase(c) || isLowerCase(c) || isNumeric(c);
}

static inline void skipWhiteSpaces(const char** src) {
    while (isWhiteSpace(**src)) {
        if ((**src) == '\n') {
            current_line_number++;
        }
        (*src)++;
    }
}

static inline void skipOneLineComment(const char** src) {
    current_line_number++;
    while ((**src) != '\n') {
        (*src)++;
    }
    (*src)++;
}

static inline void skipMultiLineComment(const char** src) {
    while (!((**src) == '/' && (*((*src) - 1)) == '*')) {
        if ((**src) == '\n') {
            current_line_number++;
        }
        (*src)++;
    }
    (*src)++;
}

static inline bool isKeyword(const char* keyword, const char* src) {
    return (strncmp(keyword, src, strlen(keyword)) == 0)
        && !isAlfaNumeric(*(src + strlen(keyword)));
}

static inline struct Token tokenKeyword(
    const char* src,
    const char** end,
    const char* keyword
) {
    (*end) = src + strlen(keyword);
    return (struct Token) {
        .file_name   = (char*)current_file,
        .line_number = current_line_number,
        .row_string  = newStringL(src, strlen(keyword)),
    };
}

static void skipComments(const char** src) {
    while (true) {
        if (isWhiteSpace(**src)) {
            skipWhiteSpaces(src);
            continue;
        }

        if ((**src) == '/' && (*((*src) + 1)) == '/') {
            skipOneLineComment(src);
            continue;
        }

        if ((**src) == '/' && (*((*src) + 1)) == '*') {
            skipMultiLineComment(src);
            continue;
        }

        break;
    }
}

static struct Token isTokenNumber(const char* src, const char** end) {
    uint64_t res = strtoull(src, (char**)end, 0);
    if (((**end) == '.') || ((**end) == 'E') || ((**end) == 'e')) {
        long double res = strtold(src, (char**)end);
        return (struct Token) {
            .file_name   = (char*)current_file,
            .line_number = current_line_number,
            .row_string  = newStringL(src, ((*end) - src)),
            .value_type  = TOKEN_FLOAT,
            .value_float = res
        };
    }
    return (struct Token) {
        .file_name   = (char*)current_file,
        .line_number = current_line_number,
        .row_string  = newStringL(src, ((*end) - src)),
        .value_type  = TOKEN_INT,
        .value_int   = res
    };
}

static struct Token isTokenUpperName(char* src, const char** end) {
    size_t length = 1; 
    while (isAlfaNumeric(src[length])) {
        length++;
    }

    (*end) = src + length;
    return (struct Token) {
        .file_name    = (char*)current_file,
        .line_number  = current_line_number,
        .row_string   = newStringL(src, length),
        .value_type   = TOKEN_UPPER_NAME,
        .value_string = newStringL(src, length) 
    };
}

static struct Token isTokenLowerName(const char* src, const char** end) {
    if (isKeyword("import", src)) {
        struct Token res = tokenKeyword(src, end, "import");
        res.value_type = TOKEN_KEYWORD_IMPORT;           // import
        return res;
    }  
    
    if (isKeyword("as", src)) {
        struct Token res = tokenKeyword(src, end, "as");
        res.value_type = TOKEN_KEYWORD_AS;               // as
        return res;
    }
    
    if (isKeyword("type", src)) {
        struct Token res = tokenKeyword(src, end, "type");
        res.value_type = TOKEN_KEYWORD_TYPE;             // type
        return res;
    }
    
    if (isKeyword("enum", src)) {
        struct Token res = tokenKeyword(src, end, "enum");
        res.value_type = TOKEN_KEYWORD_ENUM;             // enum
        return res;
    }
    
    if (isKeyword("union", src)) {
        struct Token res = tokenKeyword(src, end, "union");
        res.value_type = TOKEN_KEYWORD_UNION;            // union
        return res;
    }
    
    if (isKeyword("export", src)) {
        struct Token res = tokenKeyword(src, end, "export");
        res.value_type = TOKEN_KEYWORD_EXPORT;           // export
        return res;
    }
    
    if (isKeyword("external", src)) {
        struct Token res = tokenKeyword(src, end, "external");
        res.value_type = TOKEN_KEYWORD_EXTERNAL;         // external
        return res;
    }
    
    if (isKeyword("var", src)) {
        struct Token res = tokenKeyword(src, end, "var");
        res.value_type = TOKEN_KEYWORD_VAR;              // var
        return res;
    }
    
    if (isKeyword("const", src)) {
        struct Token res = tokenKeyword(src, end, "const");
        res.value_type = TOKEN_KEYWORD_CONST;            // const
        return res;
    }

    if (isKeyword("ref", src)) {
        struct Token res = tokenKeyword(src, end, "ref");
        res.value_type = TOKEN_KEYWORD_REF;              // ref
        return res;
    }
    
    if (isKeyword("func", src)) {
        struct Token res = tokenKeyword(src, end, "func");
        res.value_type = TOKEN_KEYWORD_FUNC;             // func
        return res;
    }
    
    if (isKeyword("cfunc", src)) {
        struct Token res = tokenKeyword(src, end, "cfunc");
        res.value_type = TOKEN_KEYWORD_CFUNC;            // cfunc
        return res;
    }
    
    if (isKeyword("if", src)) {
        struct Token res = tokenKeyword(src, end, "if");
        res.value_type = TOKEN_KEYWORD_IF;               // if
        return res;
    }
    
    if (isKeyword("else", src)) {
        struct Token res = tokenKeyword(src, end, "else");
        res.value_type = TOKEN_KEYWORD_ELSE;             // else
        return res;
    }
    
    if (isKeyword("switch", src)) {
        struct Token res = tokenKeyword(src, end, "switch");
        res.value_type = TOKEN_KEYWORD_SWITCH;           // switch
        return res;
    }
    
    if (isKeyword("case", src)) {
        struct Token res = tokenKeyword(src, end, "case");
        res.value_type = TOKEN_KEYWORD_CASE;             // case
        return res;
    }
    
    if (isKeyword("default", src)) {
        struct Token res = tokenKeyword(src, end, "default");
        res.value_type = TOKEN_KEYWORD_DEFAULT;          // default
        return res;
    }
    
    if (isKeyword("loop", src)) {
        struct Token res = tokenKeyword(src, end, "loop");
        res.value_type = TOKEN_KEYWORD_LOOP;             // loop
        return res;
    }
    
    if (isKeyword("break", src)) {
        struct Token res = tokenKeyword(src, end, "break");
        res.value_type = TOKEN_KEYWORD_BREAK;            // break
        return res;
    }
    
    if (isKeyword("continue", src)) {
        struct Token res = tokenKeyword(src, end, "continue");
        res.value_type = TOKEN_KEYWORD_CONTINUE;         // continue
        return res;
    }
    
    if (isKeyword("do", src)) {
        struct Token res = tokenKeyword(src, end, "do");
        res.value_type = TOKEN_KEYWORD_DO;               // do
        return res;
    }
    
    if (isKeyword("while", src)) {
        struct Token res = tokenKeyword(src, end, "while");
        res.value_type = TOKEN_KEYWORD_WHILE;            // while
        return res;
    }
    
    if (isKeyword("for", src)) {
        struct Token res = tokenKeyword(src, end, "for");
        res.value_type = TOKEN_KEYWORD_FOR;              // for
        return res;
    }
    
    if (isKeyword("foreatch", src)) {
        struct Token res = tokenKeyword(src, end, "foreatch");
        res.value_type = TOKEN_KEYWORD_FOREATCH;         // foreatch
        return res;
    }
    
    if (isKeyword("repead", src)) {
        struct Token res = tokenKeyword(src, end, "repead");
        res.value_type = TOKEN_KEYWORD_REPEAD;           // repead
        return res;
    }
    
    if (isKeyword("return", src)) {
        struct Token res = tokenKeyword(src, end, "return");
        res.value_type = TOKEN_KEYWORD_RETURN;           // return
        return res;
    }
    
    if (isKeyword("test", src)) {
        struct Token res = tokenKeyword(src, end, "test");
        res.value_type = TOKEN_KEYWORD_TEST;             // test
        return res;
    }

    size_t length = 1; 
    while (isAlfaNumeric(src[length])) {
        length++;
    }
    (*end) = src + length;
    return (struct Token) {
        .file_name    = (char*)current_file,
        .line_number  = current_line_number,
        .row_string   = newStringL(src, length),
        .value_type   = TOKEN_LOWER_NAME,
        .value_string = newStringL(src, length) 
    };
}

static struct Token isTokenDotName(const char* src, const char** end) {
    if (!(isLowerCase(src[1]))) {
        errorIlligalName();
    }
    size_t length = 1; 
    while (isAlfaNumeric(src[length])) {
        length++;
    }
    (*end) = src + length;
    return (struct Token) {
        .file_name    = (char*)current_file,
        .line_number  = current_line_number,
        .row_string   = newStringL(src, length),
        .value_type   = TOKEN_DOT_NAME,
        .value_string = newStringL(src, length) 
    };
}

static struct Token isTokenAtName(const char* src, const char** end) {
    if (!(isLowerCase(src[1]))) {
        errorIlligalName();
    }
    size_t length = 1; 
    while (isAlfaNumeric(src[length])) {
        length++;
    }
    (*end) = src + length;
    return (struct Token) {
        .file_name    = (char*)current_file,
        .line_number  = current_line_number,
        .row_string   = newStringL(src, length),
        .value_type   = TOKEN_DOT_NAME,
        .value_string = newStringL(src, length) 
    };
}

static char parseHexDigit(char c) {
    if ('0' <= c && c <= '9') {
        return c - '0';
    } else if ('A' <= c && c <= 'F') {
        return c - 'A' + 10;
    } else if ('a' <= c && c <= 'f') {
        return c - 'a' + 10;
    }
    return 0;
}

static char parseHex(const char* str) {
    char res = 0;
    res = res | parseHexDigit(*str);
    res = res << 4;
    res = res | parseHexDigit(*(str + 1));
    return res;
}

static struct String escape(const char* str, size_t length) {
    char* res = memoryAlloc(length + 1);
    size_t res_length = 0;
    for (size_t i = 0; i < length; i++) {
        if (str[i] == '\\') {
            switch (str[++i]) {
            case '\\':
                res[res_length++] = '\\';
                break;
            case 'b':
                res[res_length++] = '\b';
                break;
            case 'a':
                res[res_length++] = '\a';
                break;
            case 'e':
                res[res_length++] = 0x1b;
                break;
            case 'f':
                res[res_length++] = '\f';
                break;
            case 'n':
                res[res_length++] = '\n';
                break;
            case 'r':
                res[res_length++] = '\r';
                break;
            case 't':
                res[res_length++] = '\t';
                break;
            case 'v':
                res[res_length++] = '\v';
                break;
            case 'x':
                i++;
                res[res_length++] = parseHex(str + i);
                i++;
                break;
            default:
                errorIlligalEscape(str[i]);
                res[res_length++] = '/';
                res[res_length++] = str[i];
            }
        } else if (str[i] == '\n') {
            errorIlligalNewLine(); 
            break;
        } else {
            res[res_length++] = str[i];
        }
    }
    return (struct String) {
        .string = memoryStringnLengthDup(res, res_length),
        .length = res_length
    };
}

static struct Token isTokenString(const char* src, const char** end) {
    if ((*src) == '"') {
        size_t length = 1;; 
        while (!(((src[length] == '"')
               && (src[length - 1] != '\\')) 
              || ((src[length] == '"') 
               && (src[length - 1] == '\\')
               && (src[length - 2] == '\\')))) {
            length++;
        }
        length++;
        *end = src + length;
        struct String res = escape(src + 1, length - 2);
        return (struct Token) {
            .file_name    = (char*)current_file,
            .line_number  = current_line_number,
            .row_string   = newStringL(src, length),
            .value_type   = TOKEN_STRING,
            .value_string = res 
        };
    }
    return (struct Token) { 0 };
}

static struct Token isTokenSpecjalc(const char* src, const char** end) {
    switch (*src) {
    case '(':
        (*end) = src + 1;
        return (struct Token) {
            .file_name    = (char*)current_file,
            .line_number  = current_line_number,
            .row_string   = newStringL(src, 1),
            .value_type   = TOKEN_PARENTHESES_OPEN,
            .value_string = newStringL(src, 1),
        };
    case ')':
        (*end) = src + 1;
        return (struct Token) {
            .file_name    = (char*)current_file,
            .line_number  = current_line_number,
            .row_string   = newStringL(src, 1),
            .value_type   = TOKEN_PARENTHESES_CLOSE,
            .value_string = newStringL(src, 1),
        };
    case '{':
        (*end) = src + 1;
        return (struct Token) {
            .file_name    = (char*)current_file,
            .line_number  = current_line_number,
            .row_string   = newStringL(src, 1),
            .value_type   = TOKEN_BRACES_OPEN,
            .value_string = newStringL(src, 1),
        };
    case '}':
        (*end) = src + 1;
        return (struct Token) {
            .file_name    = (char*)current_file,
            .line_number  = current_line_number,
            .row_string   = newStringL(src, 1),
            .value_type   = TOKEN_BRACES_CLOSE,
            .value_string = newStringL(src, 1),
        };
    case '[':
        (*end) = src + 1;
        return (struct Token) {
            .file_name    = (char*)current_file,
            .line_number  = current_line_number,
            .row_string   = newStringL(src, 1),
            .value_type   = TOKEN_BRACKETS_OPEN,
            .value_string = newStringL(src, 1),
        };
    case ']':
        (*end) = src + 1;
        return (struct Token) {
            .file_name    = (char*)current_file,
            .line_number  = current_line_number,
            .row_string   = newStringL(src, 1),
            .value_type   = TOKEN_BRACKETS_CLOSE,
            .value_string = newStringL(src, 1),
        };
    case ';':
        (*end) = src + 1;
        return (struct Token) {
            .file_name    = (char*)current_file,
            .line_number  = current_line_number,
            .row_string   = newStringL(src, 1),
            .value_type   = TOKEN_SEMICOLON,
            .value_string = newStringL(src, 1),
        };
    case ':':
        (*end) = src + 1;
        return (struct Token) {
            .file_name    = (char*)current_file,
            .line_number  = current_line_number,
            .row_string   = newStringL(src, 1),
            .value_type   = TOKEN_COLON,
            .value_string = newStringL(src, 1),
        };
    case ',':
        (*end) = src + 1;
        return (struct Token) {
            .file_name    = (char*)current_file,
            .line_number  = current_line_number,
            .row_string   = newStringL(src, 1),
            .value_type   = TOKEN_COMMA,
            .value_string = newStringL(src, 1),
        };
    case '<':
        switch (*(src + 1)) {
        case '=':
            (*end) = src + 2;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 2),
                .value_type   = TOKEN_LESS_OR_EQUAL_THEN,
                .value_string = newStringL(src, 2),
            };
        case '<':
            switch (*(src + 2)) {
            case '=':
                (*end) = src + 3;
                return (struct Token) {
                    .file_name    = (char*)current_file,
                    .line_number  = current_line_number,
                    .row_string   = newStringL(src, 3),
                    .value_type   = TOKEN_ASIGN_SHIFT_LEFT,
                    .value_string = newStringL(src, 3),
                };
            default:
                (*end) = src + 2;
                return (struct Token) {
                    .file_name    = (char*)current_file,
                    .line_number  = current_line_number,
                    .row_string   = newStringL(src, 1),
                    .value_type   = TOKEN_SHIFT_LEFT,
                    .value_string = newStringL(src, 1),
                };
            }
        default:
            (*end) = src + 1;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 1),
                .value_type   = TOKEN_LESS_THEN,
                .value_string = newStringL(src, 1),
            };
        }
    case '>':
        switch (*(src + 1)) {
        case '=':
            (*end) = src + 2;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 2),
                .value_type   = TOKEN_GREATER_OR_EQUAL_THEN,
                .value_string = newStringL(src, 2),
            };
        case '>':
            switch (*(src + 2)) {
            case '=':
                (*end) = src + 3;
                return (struct Token) {
                    .file_name    = (char*)current_file,
                    .line_number  = current_line_number,
                    .row_string   = newStringL(src, 3),
                    .value_type   = TOKEN_ASIGN_SHIFT_RIGHT,
                    .value_string = newStringL(src, 3),
                };
            default:
                (*end) = src + 2;
                return (struct Token) {
                    .file_name    = (char*)current_file,
                    .line_number  = current_line_number,
                    .row_string   = newStringL(src, 1),
                    .value_type   = TOKEN_SHIFT_RIGHT,
                    .value_string = newStringL(src, 1),
                };
            }
        default:
            (*end) = src + 1;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 1),
                .value_type   = TOKEN_GREATER_THEN,
                .value_string = newStringL(src, 1),
            };
        }

    case '=':
        switch (*(src + 1)) {
        case '=':
            (*end) = src + 2;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 2),
                .value_type   = TOKEN_EQUAL,
                .value_string = newStringL(src, 2),
            };
        default:
            (*end) = src + 1;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 1),
                .value_type   = TOKEN_ASIGN,
                .value_string = newStringL(src, 1),
            };
        }
    case '!':
        switch (*(src + 1)) {
        case '=':
            (*end) = src + 2;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 2),
                .value_type   = TOKEN_NOT_EQUAL,
                .value_string = newStringL(src, 2),
            };
        default:
            (*end) = src + 1;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 1),
                .value_type   = TOKEN_LOGICAL_NOT,
                .value_string = newStringL(src, 1),
            };
        }
    case '~':
        switch (*(src + 1)) {
        case '=':
            (*end) = src + 2;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 2),
                .value_type   = TOKEN_ASIGN_BITWIZE_NOT,
                .value_string = newStringL(src, 2),
            };
        default:
            (*end) = src + 1;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 1),
                .value_type   = TOKEN_BITWIZE_NOT,
                .value_string = newStringL(src, 1),
            };
        }
    case '|':
        switch (*(src + 1)) {
        case '|':
            (*end) = src + 2;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 2),
                .value_type   = TOKEN_LOGICAL_OR,
                .value_string = newStringL(src, 2),
            };
        case '=':
            (*end) = src + 2;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 2),
                .value_type   = TOKEN_ASIGN_BITWIZE_OR,
                .value_string = newStringL(src, 2),
            };
        default:
            (*end) = src + 1;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 1),
                .value_type   = TOKEN_BITWIZE_OR,
                .value_string = newStringL(src, 1),
            };
        }
    case '&':
        switch (*(src + 1)) {
        case '&':
            (*end) = src + 2;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 2),
                .value_type   = TOKEN_LOGICAL_AND,
                .value_string = newStringL(src, 2),
            };
        case '=':
            (*end) = src + 2;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 2),
                .value_type   = TOKEN_ASIGN_BITWIZE_AND,
                .value_string = newStringL(src, 2),
            };
        default:
            (*end) = src + 1;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 1),
                .value_type   = TOKEN_BITWIZE_AND,
                .value_string = newStringL(src, 1),
            };
        }
    case '+':
        switch (*(src + 1)) {
        case '=':
            (*end) = src + 2;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 2),
                .value_type   = TOKEN_ASIGN_PLUS,
                .value_string = newStringL(src, 2),
            };
        default:
            (*end) = src + 1;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 1),
                .value_type   = TOKEN_PLUS,
                .value_string = newStringL(src, 1),
            };
        }
    case '-':
        switch (*(src + 1)) {
        case '=':
            (*end) = src + 2;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 2),
                .value_type   = TOKEN_ASIGN_MINUS,
                .value_string = newStringL(src, 2),
            };
        default:
            (*end) = src + 1;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 1),
                .value_type   = TOKEN_MINUS,
                .value_string = newStringL(src, 1),
            };
        }
    case '*':
        switch (*(src + 1)) {
        case '=':
            (*end) = src + 2;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 2),
                .value_type   = TOKEN_ASIGN_MULTIPLY,
                .value_string = newStringL(src, 2),
            };
        default:
            (*end) = src + 1;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 1),
                .value_type   = TOKEN_MULTIPLY,
                .value_string = newStringL(src, 1),
            };
        }
    case '/':
        switch (*(src + 1)) {
        case '=':
            (*end) = src + 2;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 2),
                .value_type   = TOKEN_ASIGN_DIVIDE,
                .value_string = newStringL(src, 2),
            };
        default:
            (*end) = src + 1;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 1),
                .value_type   = TOKEN_DIVIDE,
                .value_string = newStringL(src, 1),
            };
        }
    case '%':
        switch (*(src + 1)) {
        case '=':
            (*end) = src + 2;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 2),
                .value_type   = TOKEN_ASIGN_MODULO,
                .value_string = newStringL(src, 2),
            };
        default:
            (*end) = src + 1;
            return (struct Token) {
                .file_name    = (char*)current_file,
                .line_number  = current_line_number,
                .row_string   = newStringL(src, 1),
                .value_type   = TOKEN_MODULO,
                .value_string = newStringL(src, 1),
            };
        }
    }

    errorIlligalCharacter(*src);
    // becouse: warning: control reaches end of non-void function
    return (struct Token) { 0 }; 
}

static void appendToken(struct TokenList **end_of_tokens, struct Token new) {
    (*end_of_tokens) -> token = new;
    (*end_of_tokens) -> next = memoryAlloc(sizeof(struct TokenList));
    (*end_of_tokens) = (*end_of_tokens) -> next;
}

struct TokenList* getTokens(const char* src, const char* file_name) {
    current_line_number = 1;
    current_file = file_name;
    struct TokenList* res = memoryAlloc(sizeof(struct TokenList));
    struct TokenList* current = res;
    while (true) {
        skipComments(&src);
        if ((*src) == 0) {
            break;
        }

        switch (*src) {
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': 
        case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': 
        case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
        case 'V': case 'W': case 'X': case 'Y': case 'Z':
            appendToken(&current, isTokenUpperName((char*)src, &src));
            break;
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': 
        case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': 
        case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
        case 'v': case 'w': case 'x': case 'y': case 'z':
            appendToken(&current, isTokenLowerName((char*)src, &src));
            break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            appendToken(&current, isTokenNumber((char*)src, &src));
            break;
        case '.':
            appendToken(&current, isTokenDotName((char*)src, &src));
            break;
        case '@':
            appendToken(&current, isTokenAtName((char*)src, &src));
            break;
        case '"':
            appendToken(&current, isTokenString((char*)src, &src));
            break;
        default:
            appendToken(&current, isTokenSpecjalc((char*)src, &src));
            break;
        }
    }
    return res;
}

void freeTokens(struct TokenList* tokens) {
    while (true) {
        if (tokens -> next == NULL) {
            memoryFree(tokens);
            break;
        }
        struct TokenList* old = tokens;
        tokens = tokens -> next;
        memoryFree(old);
    }
}

void printToken(struct Token* token) {
    switch (token -> value_type) {
    case TOKEN_NONE:
        // that wuld never hpaend.
        break;
    
    case TOKEN_UPPER_NAME:
        printf(
            "TOKEN_UPPER_NAME: %.*s\n",
            (int) token -> value_string.length,
            token -> value_string.string
        );
        break;
    case TOKEN_LOWER_NAME:
        printf(
            "TOKEN_LOWER_NAME: %.*s\n", 
            (int) token -> value_string.length,
            token -> value_string.string
        );
        break;
    case TOKEN_DOT_NAME:
        printf(
            "TOKEN_DOT_NAME: %.*s\n", 
            (int) token -> value_string.length,
            token -> value_string.string
        );
        break;
    case TOKEN_AT_NAME:
        printf(
            "TOKEN_DOT_NAME: %.*s\n", 
            (int) token -> value_string.length,
            token -> value_string.string
        );
        break;
    case TOKEN_STRING:
        printf(
            "TOKEN_STRING: \"%.*s\"\n", 
            (int) token -> value_string.length,
            token -> value_string.string
        );
        break;
    
    case TOKEN_KEYWORD_IMPORT:
        printf("TOKEN_KEYWORD_IMPORT\n");
        break;
    case TOKEN_KEYWORD_AS:
        printf("TOKEN_KEYWORD_AS\n");
        break;
    case TOKEN_KEYWORD_TYPE:
        printf("TOKEN_KEYWORD_TYPE\n");
        break;
    case TOKEN_KEYWORD_ENUM:
        printf("TOKEN_KEYWORD_ENUM\n");
        break;
    case TOKEN_KEYWORD_UNION:
        printf("TOKEN_KEYWORD_UNION\n");
        break;
    case TOKEN_KEYWORD_EXPORT:
        printf("TOKEN_KEYWORD_EXPORT\n");
        break;
    case TOKEN_KEYWORD_EXTERNAL:
        printf("TOKEN_KEYWORD_EXTERNAL\n");
        break;
    case TOKEN_KEYWORD_VAR:
        printf("TOKEN_KEYWORD_VAR\n");
        break;
    case TOKEN_KEYWORD_CONST:
        printf("TOKEN_KEYWORD_CONST\n");
        break;
    case TOKEN_KEYWORD_REF:
        printf("TOKEN_KEYWORD_REF\n");
        break;
    case TOKEN_KEYWORD_FUNC:
        printf("TOKEN_KEYWORD_FUNC\n");
        break;
    case TOKEN_KEYWORD_CFUNC:
        printf("TOKEN_KEYWORD_CFUNC\n");
        break;
    case TOKEN_KEYWORD_IF:
        printf("TOKEN_KEYWORD_IF\n");
        break;
    case TOKEN_KEYWORD_ELSE:
        printf("TOKEN_KEYWORD_ELSE\n");
        break;
    case TOKEN_KEYWORD_SWITCH:
        printf("TOKEN_KEYWORD_SWITCH\n");
        break;
    case TOKEN_KEYWORD_CASE:
        printf("TOKEN_KEYWORD_CASE\n");
        break;
    case TOKEN_KEYWORD_DEFAULT:
        printf("TOKEN_KEYWORD_DEFAULT\n");
        break;
    case TOKEN_KEYWORD_LOOP:
        printf("TOKEN_KEYWORD_LOOP\n");
        break;
    case TOKEN_KEYWORD_BREAK:
        printf("TOKEN_KEYWORD_BREAK\n");
        break;
    case TOKEN_KEYWORD_CONTINUE:
        printf("TOKEN_KEYWORD_CONTINUE\n");
        break;
    case TOKEN_KEYWORD_DO:
        printf("TOKEN_KEYWORD_DO\n");
        break;
    case TOKEN_KEYWORD_WHILE:
        printf("TOKEN_KEYWORD_WHILE\n");
        break;
    case TOKEN_KEYWORD_FOR:
        printf("TOKEN_KEYWORD_FOR\n");
        break;
    case TOKEN_KEYWORD_FOREATCH:
        printf("TOKEN_KEYWORD_FOREATCH\n");
        break;
    case TOKEN_KEYWORD_REPEAD:
        printf("TOKEN_KEYWORD_REPEAD\n");
        break;
    case TOKEN_KEYWORD_RETURN:
        printf("TOKEN_KEYWORD_RETURN\n");
        break;
    case TOKEN_KEYWORD_TEST:
        printf("TOKEN_KEYWORD_TEST\n");
        break;
    
    case TOKEN_FLOAT:
        printf("TOKEN_FLOAT: %Lg\n", token -> value_float);
        break;
    case TOKEN_INT:
        printf("TOKEN_INT: %ld\n", token -> value_int);
        break;
    
    case TOKEN_PARENTHESES_OPEN:
        printf("TOKEN_PARENTHESES_OPEN\n");
        break;
    case TOKEN_PARENTHESES_CLOSE:
        printf("TOKEN_PARENTHESES_CLOSE\n");
        break;
    case TOKEN_BRACES_OPEN:
        printf("TOKEN_BRACES_OPEN\n");
        break;
    case TOKEN_BRACES_CLOSE:
        printf("TOKEN_BRACES_CLOSE\n");
        break;
    case TOKEN_BRACKETS_OPEN:
        printf("TOKEN_BRACKETS_OPEN\n");
        break;
    case TOKEN_BRACKETS_CLOSE:
        printf("TOKEN_BRACKETS_CLOSE\n");
        break;
    
    case TOKEN_SEMICOLON:
        printf("TOKEN_SEMICOLON\n");
        break;
    case TOKEN_COLON:
        printf("TOKEN_COLON\n");
        break;
    case TOKEN_COMMA:
        printf("TOKEN_COMMA\n");
        break;
    
    case TOKEN_LESS_THEN:
        printf("TOKEN_LESS_THEN\n");
        break;
    case TOKEN_GREATER_THEN:
        printf("TOKEN_GREATER_THEN\n");
        break;
    case TOKEN_LESS_OR_EQUAL_THEN:
        printf("TOKEN_LESS_OR_EQUAL_THEN\n");
        break;
    case TOKEN_GREATER_OR_EQUAL_THEN:
        printf("TOKEN_GREATER_OR_EQUAL_THEN\n");
        break;
    case TOKEN_EQUAL:
        printf("TOKEN_EQUAL\n");
        break;
    case TOKEN_NOT_EQUAL:
        printf("TOKEN_NOT_EQUAL\n");
        break;
    
    case TOKEN_LOGICAL_NOT:
        printf("TOKEN_LOGICAL_NOT\n");
        break;
    case TOKEN_LOGICAL_OR:
        printf("TOKEN_LOGICAL_OR\n");
        break;
    case TOKEN_LOGICAL_AND:
        printf("TOKEN_LOGICAL_AND\n");
        break;
    
    case TOKEN_SHIFT_LEFT:
        printf("TOKEN_SHIFT_LEFT\n");
        break;
    case TOKEN_SHIFT_RIGHT:
        printf("TOKEN_SHIFT_RIGHT\n");
        break;
    
    case TOKEN_BITWIZE_NOT:
        printf("TOKEN_BITWIZE_NOT\n");
        break;
    case TOKEN_BITWIZE_OR:
        printf("TOKEN_BITWIZE_OR\n");
        break;
    case TOKEN_BITWIZE_AND:
        printf("TOKEN_BITWIZE_AND\n");
        break;
    
    case TOKEN_PLUS:
        printf("TOKEN_PLUS\n");
        break;
    case TOKEN_MINUS:
        printf("TOKEN_MINUS\n");
        break;
    case TOKEN_MULTIPLY:
        printf("TOKEN_MULTIPLY\n");
        break;
    case TOKEN_DIVIDE:
        printf("TOKEN_DIVIDE\n");
        break;
    case TOKEN_MODULO:
        printf("TOKEN_MODULO\n");
        break;
    
    case TOKEN_ASIGN:
        printf("TOKEN_ASIGN\n");
        break;
    
    case TOKEN_ASIGN_SHIFT_LEFT:
        printf("TOKEN_ASIGN_SHIFT_LEFT\n");
        break;
    case TOKEN_ASIGN_SHIFT_RIGHT:
        printf("TOKEN_ASIGN_SHIFT_RIGHT\n");
        break;
    
    case TOKEN_ASIGN_BITWIZE_NOT:
        printf("TOKEN_ASIGN_BITWIZE_NOT\n");
        break;
    case TOKEN_ASIGN_BITWIZE_OR:
        printf("TOKEN_ASIGN_BITWIZE_OR\n");
        break;
    case TOKEN_ASIGN_BITWIZE_AND:
        printf("TOKEN_ASIGN_BITWIZE_AND\n");
        break;
    
    case TOKEN_ASIGN_PLUS:
        printf("TOKEN_ASIGN_PLUS\n");
        break;
    case TOKEN_ASIGN_MINUS:
        printf("TOKEN_ASIGN_MINUS\n");
        break;
    case TOKEN_ASIGN_MULTIPLY:
        printf("TOKEN_ASIGN_MULTIPLY\n");
        break;
    case TOKEN_ASIGN_DIVIDE:
        printf("TOKEN_ASIGN_DIVIDE\n");
        break;
    case TOKEN_ASIGN_MODULO:
        printf("TOKEN_ASIGN_MODULO\n");
        break;
    }
}

void printTokens(struct TokenList* tokens) {
    while (true) {
        printToken(&tokens -> token);
        if (tokens -> next == NULL) {
            break;
        }
        tokens = tokens -> next;
    }
}
