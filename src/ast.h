#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "string.h"
#include "memory.h"

struct Path {
    struct String name;
    struct Path*  next;
};

struct Import {
    bool          is_rename;
    struct String as;
    struct Path*  path;
};

/*      
 * Type <Type, Type> | TypeHeader
 *      ^- TypeParams
 * Type <Type<Type>> | Type
 *      ^- TypeArgs
 */

struct TypeHeader {
    struct String      name;
    struct TypeParams* params;
};

struct TypeParams {
    struct String      name;
    struct TypeParams* next;
};

struct Type {
    bool             is_ref;
    struct String    name;
    struct TypeArgs* args;
};

struct TypeArgs {
    struct Type      type;
    struct TypeArgs* next;
};

struct TypeFild {
    struct String name;
    struct Type   type;
};

struct TypeFildList {
    struct TypeFild      type;
    struct TypeFildList* next;
};

enum EnumFildListType {
    ENUM_FILD_UNTYPED,
    ENUM_FILD_TYPED,
};

struct EnumFildList {
    enum EnumFildListType type;
    union {
        struct String   untyped;
        struct TypeFild typed;
    };
    struct EnumFildList* next;
};


enum TypeDeclType {
    TYPE_TYPE,
    TYPE_ENUM,
    TYPE_UNION,
    TYPE_STRUCT,
};

struct TypeDecl {
    bool              is_exported;
    struct TypeHeader header;
    enum TypeDeclType type;
    union {
        struct Type          _type;
        struct TypeFildList* _struct;
        struct TypeFildList* _union;
        struct EnumFildList* _enum;
    };
};

enum LiteralType {
    LITERAL_STING,
    LITERAL_FLOAT,
    LITERAL_INT,
    LITERAL_NAME,
};

struct Literal {
    enum LiteralType type;
    union {
        struct String string;
        struct Path*  name;
        long double   _float;
        uint64_t      _int;
    };
};

enum ExpretionTypy {
    EXPRETION_NONE,
    EXPRETION_CAST,
    EXPRETION_REF,
    EXPRETION_DEREF,
    EXPRETION_GET,
    EXPRETION_NEG,

    EXPRETION_ADD,
    EXPRETION_SUBTRACT,
    EXPRETION_MULTIPLY,
    EXPRETION_DIVIDE,
    EXPRETION_MODULO,

    EXPRETION_EQUAL,
    EXPRETION_LESS_THEN,
    EXPRETION_GREAT_THEN,
    EXPRETION_NOT_EQUAL,
    EXPRETION_LESS_THEN_OR_EQUAL,
    EXPRETION_GREA_THEN_OR_EQUAL,

    EXPRETION_BITWIZE_NOT,
    EXPRETION_BITWIZE_OR,
    EXPRETION_BITWIZE_AND,

    EXPRETION_LEFT_SHIFT,
    EXPRETION_RIGHT_SHIFT,

    EXPRETION_LOGICAL_NOT,
    EXPRETION_LOGICAL_AND,
    EXPRETION_LOGICAL_OR,

    EXPRETION_FUNCTION,
    EXPRETION_LITERAL,
};

struct FunctionCall {
    struct String        name;
    struct FunctionArgs* args;
};

struct Expretion {
    enum ExpretionTypy type;
    union {
        struct {
            struct Expretion* left;
            struct Expretion* right;
        };
        struct {
            struct Expretion* expr;
            struct Type       cast;
        };
        struct FunctionCall func;
        struct Literal      literal;
    };
};

struct FunctionArgs {
    struct Expretion     arg;
    struct FunctionArgs* next;
};

struct StatementVar {
    struct String     name;
    struct Expretion* value;
};

struct StatementConst {
    struct String     name;
    struct Expretion* value;
};

struct StatementIf {
    struct Expretion*     condition;
    struct StatementBloc* then;
    struct StatementBloc* _else;
};

struct StatementSwitchCase {
    bool                       _default;
    struct Literal             _case;
    struct StatementBloc*      then;
    struct StatementSwitchCase* next;
};

struct StatementSwitch {
    struct StatementSwitchCase* cases;
};

struct StatementDo {
    bool                  is_label;
    struct String         label;
    struct Expretion*     condition;
    struct StatementBloc* then;
};

struct StatementWhile {
    bool                  is_label;
    struct String         label;
    struct Expretion*     condition;
    struct StatementBloc* then;
};

struct StatementFor {
    bool                  is_label;
    struct String         label;
    struct String         var;
    struct Expretion*     condition;
    struct StatementBloc* then;
};

struct StatementRepead {
    bool                  is_label;
    struct String         label;
    struct Expretion*     condition;
    struct StatementBloc* then;
};

struct StatementReturn {
    struct Expretion* value;
};

struct StatementAsign {
    struct Expretion* get_expr;
    struct String     var_name;
    struct Expretion* value;
};

enum StatementType {
    STATEMENT_NONE,
    STATEMENT_VAR,
    STATEMENT_CONST,
    STATEMENT_IF,
    STATEMENT_SWITCH,
    STATEMENT_DO,
    STATEMENT_WHILE,
    STATEMENT_FOR,
    STATEMENT_REPEAD,
    STATEMENT_RETURN,
    STATEMENT_ASIGN,
    STATEMENT_CALL,
};

struct Statement {
    enum StatementType type;
    union {
        struct StatementVar*     statement_var;
        struct StatementConst*   statement_const;
        struct StatementIf*      statement_if;
        struct StatementSwitch*  statement_switch;
        struct StatementDo*      statement_do;
        struct StatementWhile*   statement_while;
        struct StatementFor*     statement_for;
        struct StatementRepead*  statement_repead;
        struct StatementReturn*  statement_return;
        struct StatementAsign*   statement_asign;
        struct FunctionCall*     statement_expr;
    };
};

struct StatementBloc {
    struct Statement      expr;
    struct StatementBloc* next;
};

struct Self {
    struct String     name;
    struct TypeHeader type;
};

struct FuncDecl {
    bool                  is_exported;
    bool                  is_external;
    struct Self*          self;
    struct String         name;
    struct TypeFildList*  args;
    struct Statement*     body;
};

struct CFuncDecl {
    struct String         name;
    struct TypeFildList*  args;
    struct Statement*     body;
};

struct Test {
    struct Statement* body;
};

enum ASTType {
    AST_NONE,
    AST_IMPORT,
    AST_TEST,
    AST_TYPE,
    AST_FUNC,
    AST_CFUNC,
};

struct AST {
    enum ASTType type;
    union {
        struct Import     ast_import;
        struct Test       ast_test;
        struct TypeDecl   ast_type;
        struct FuncDecl   ast_func;
        struct CFuncDecl  ast_cfunc;
    };
    struct AST* next;
};

static inline struct Expretion* expretionCast(
    struct Expretion* expr,
    struct Type       cast
);
static inline struct Expretion* expretionRef(struct Expretion* expr); 
static inline struct Expretion* expretionDeref(struct Expretion* expr);
static inline struct Expretion* expretionGet(struct Expretion* expr);
static inline struct Expretion* expretionNeg(struct Expretion* expr);
static inline struct Expretion* expretionAdd(
    struct Expretion* left,
    struct Expretion* right
);
static inline struct Expretion* expretionSubtract(
    struct Expretion* left,
    struct Expretion* right
);
static inline struct Expretion* expretionMultiply(
    struct Expretion* left,
    struct Expretion* right
);
static inline struct Expretion* expretionDivide(
    struct Expretion* left,
    struct Expretion* right
);
static inline struct Expretion* expretionModulo(
    struct Expretion* left,
    struct Expretion* right
);
static inline struct Expretion* expretionEqual(
    struct Expretion* left,
    struct Expretion* right
);
static inline struct Expretion* expretionLessThen(
    struct Expretion* left,
    struct Expretion* right
);
static inline struct Expretion* expretionGreatThen(
    struct Expretion* left,
    struct Expretion* right
);
static inline struct Expretion* expretionNotEqual(
    struct Expretion* left,
    struct Expretion* right
);
static inline struct Expretion* expretionLessThenOrAqual(
    struct Expretion* left,
    struct Expretion* right
);
static inline struct Expretion* expretionGreaThenOrEqual(
    struct Expretion* left,
    struct Expretion* right
);
static inline struct Expretion* expretionBitwizeNot(struct Expretion* expr);
static inline struct Expretion* expretionBitwizeOr(
    struct Expretion* left,
    struct Expretion* right
);
static inline struct Expretion* expretionBitwizeAnd(
    struct Expretion* left,
    struct Expretion* right
);
static inline struct Expretion* expretionLeftShift(
    struct Expretion* left,
    struct Expretion* right
);
static inline struct Expretion* expretionRightShift(
    struct Expretion* left,
    struct Expretion* right
);
static inline struct Expretion* expretionLogicalNot(struct Expretion* expr);
static inline struct Expretion* expretionLogicalAnd(
    struct Expretion* left,
    struct Expretion* right
);
static inline struct Expretion* expretionLogicalOr(
    struct Expretion* left,
    struct Expretion* right
);
static inline struct Expretion* expretionFunction(
    struct String        name,
    struct FunctionArgs* args
);
static inline struct Expretion* literalSting(struct String string);
static inline struct Expretion* literalFloat(long double _float);
static inline struct Expretion* literalInt(uint64_t _int);
static inline struct Expretion* literalName(struct Path* name);
static inline struct Self* self(
    struct String     name,
    struct TypeHeader type
);
static inline void addStatementConst(
    struct StatementBloc** stm,
    struct String          name,
    struct Expretion*      value
);
static inline void addStatementVar(
    struct StatementBloc** stm,
    struct String          name,
    struct Expretion*      value
);
static inline void addStatementSwitch(
    struct StatementBloc**      stm,
    struct StatementSwitchCase* cases
);
static inline void addStatementIf(
    struct StatementBloc** stm,
    struct Expretion*      condition,
    struct StatementBloc*  then
);
static inline void addStatementDo(
    struct StatementBloc** stm,
    bool                   is_label,
    struct String          label,
    struct Expretion*      condition,
    struct StatementBloc*  then
);
static inline void addStatementWhile(
    struct StatementBloc** stm,
    bool                   is_label,
    struct String          label,
    struct Expretion*      condition,
    struct StatementBloc*  then
); 
static inline void addStatementFor(
    struct StatementBloc** stm,
    bool                   is_label,
    struct String          label,
    struct String          var,
    struct Expretion*      condition,
    struct StatementBloc*  then
);
static inline void addStatementRepead(
    struct StatementBloc** stm,
    bool                   is_label,
    struct String          label,
    struct Expretion*      conition,
    struct StatementBloc*  then
); 
static inline void addStatementReturn(
    struct StatementBloc** stm,
    struct Expretion*      value
);
static inline void addStatementAsign(
    struct StatementBloc** stm,
    struct Expretion*      get_expr,
    struct String          var_name,
    struct Expretion*      value
);
static inline void FunctionArgs(
    struct FunctionArgs** args,
    struct Expretion      arg
);
static inline void appendEnumFildUntyped(
    struct EnumFildList** _enum,
    struct String         fild
);
static inline void appendEnumFildTyped(
    struct EnumFildList** _enum,
    struct TypeFild       fild
);
static inline void appendTypeFildList(
    struct TypeFildList** filds,
    struct TypeFild       type
);
static inline void  appendTypeArgs(
    struct TypeArgs** args,
    struct Type       type
);
static inline void appendTypeParams(
    struct TypeParams** params,
    struct String       name
);
static inline void appendPath(struct Path** path, struct String name);
static inline void addImport(struct AST** ast, struct Import import);
static inline void addTest(struct AST** ast, struct Test test);
static inline void addType(struct AST** ast, struct TypeDecl type);
static inline void addFunc(struct AST** ast, struct FuncDecl func);
static inline void addCFunc(struct AST** ast, struct CFuncDecl func);

void printAST(FILE *stream, struct AST*);
void freeAST(struct AST*);

static inline struct Expretion* expretionCast(
    struct Expretion* expr,
    struct Type       cast
) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_CAST;
    res -> expr = expr;
    res -> cast = cast;
    return res;
}

static inline struct Expretion* expretionRef(struct Expretion* expr) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_REF;
    res -> expr = expr;
    return res;
}

static inline struct Expretion* expretionDeref(struct Expretion* expr) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_DEREF;
    res -> expr = expr;
    return res;
}

static inline struct Expretion* expretionGet(struct Expretion* expr) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_GET;
    res -> expr = expr;
    return res;
}

static inline struct Expretion* expretionNeg(struct Expretion* expr) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_NEG;
    res -> expr = expr;
    return res;
}

static inline struct Expretion* expretionAdd(
    struct Expretion* left,
    struct Expretion* right
) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_ADD;
    res -> left = left;
    res -> right = right;
    return res;
}

static inline struct Expretion* expretionSubtract(
    struct Expretion* left,
    struct Expretion* right
) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_SUBTRACT;
    res -> left = left;
    res -> right = right;
    return res;
}

static inline struct Expretion* expretionMultiply(
    struct Expretion* left,
    struct Expretion* right
) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_MULTIPLY;
    res -> left = left;
    res -> right = right;
    return res;
}

static inline struct Expretion* expretionDivide(
    struct Expretion* left,
    struct Expretion* right
) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_DIVIDE;
    res -> left = left;
    res -> right = right;
    return res;
}

static inline struct Expretion* expretionModulo(
    struct Expretion* left,
    struct Expretion* right
) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_MODULO;
    res -> left = left;
    res -> right = right;
    return res;
}

static inline struct Expretion* expretionEqual(
    struct Expretion* left,
    struct Expretion* right
) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_EQUAL;
    res -> left = left;
    res -> right = right;
    return res;
}

static inline struct Expretion* expretionLessThen(
    struct Expretion* left,
    struct Expretion* right
) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_LESS_THEN;
    res -> left = left;
    res -> right = right;
    return res;
}

static inline struct Expretion* expretionGreatThen(
    struct Expretion* left,
    struct Expretion* right
) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_GREAT_THEN;
    res -> left = left;
    res -> right = right;
    return res;
}

static inline struct Expretion* expretionNotEqual(
    struct Expretion* left,
    struct Expretion* right
) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_NOT_EQUAL;
    res -> left = left;
    res -> right = right;
    return res;
}

static inline struct Expretion* expretionLessThenOrAqual(
    struct Expretion* left,
    struct Expretion* right
) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_LESS_THEN_OR_EQUAL;
    res -> left = left;
    res -> right = right;
    return res;
}

static inline struct Expretion* expretionGreaThenOrEqual(
    struct Expretion* left,
    struct Expretion* right
) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_GREA_THEN_OR_EQUAL;
    res -> left = left;
    res -> right = right;
    return res;
}

static inline struct Expretion* expretionBitwizeNot(struct Expretion* expr) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_BITWIZE_NOT;
    res -> expr = expr;
    return res;
}

static inline struct Expretion* expretionBitwizeOr(
    struct Expretion* left,
    struct Expretion* right
) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_BITWIZE_OR;
    res -> left = left;
    res -> right = right;
    return res;
}

static inline struct Expretion* expretionBitwizeAnd(
    struct Expretion* left,
    struct Expretion* right
) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_BITWIZE_AND;
    res -> left = left;
    res -> right = right;
    return res;
}

static inline struct Expretion* expretionLeftShift(
    struct Expretion* left,
    struct Expretion* right
) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_LEFT_SHIFT;
    res -> left = left;
    res -> right = right;
    return res;
}

static inline struct Expretion* expretionRightShift(
    struct Expretion* left,
    struct Expretion* right
) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_RIGHT_SHIFT;
    res -> left = left;
    res -> right = right;
    return res;
}

static inline struct Expretion* expretionLogicalNot(struct Expretion* expr) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_LOGICAL_NOT;
    res -> expr = expr;
    return res;
}

static inline struct Expretion* expretionLogicalAnd(
    struct Expretion* left,
    struct Expretion* right
) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_LOGICAL_AND;
    res -> left = left;
    res -> right = right;
    return res;
}

static inline struct Expretion* expretionLogicalOr(
    struct Expretion* left,
    struct Expretion* right
) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_LOGICAL_OR;
    res -> left = left;
    res -> right = right;
    return res;
}

static inline struct Expretion* expretionFunction(
    struct String        name,
    struct FunctionArgs* args
) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_LOGICAL_OR;
    res -> func = (struct FunctionCall) {
        .name = name,
        .args = args
    };
    return res;
}

static inline struct Self* self(
    struct String     name,
    struct TypeHeader type
) {
    struct Self* res = memoryAlloc(sizeof(struct Self));
    res -> name = name;
    res -> type = type;
    return res;
}

static inline void addStatementVar(
    struct StatementBloc** stm,
    struct String          name,
    struct Expretion*      value
) {
    struct StatementVar* res = memoryAlloc(sizeof(struct StatementVar));
    res -> name = name;
    res -> value = value;
    (*stm) -> expr = (struct Statement) {
        .type = STATEMENT_VAR,
        .statement_var = res
    };
    (*stm) -> next = memoryAlloc(sizeof(struct StatementBloc));
    (*stm) = (*stm) -> next;
}

static inline void addStatementConst(
    struct StatementBloc** stm,
    struct String          name,
    struct Expretion*      value
) {
    struct StatementConst* res = memoryAlloc(sizeof(struct StatementConst));
    res -> name = name;
    res -> value = value;
    (*stm) -> expr = (struct Statement) {
        .type = STATEMENT_CONST,
        .statement_const = res
    };
    (*stm) -> next = memoryAlloc(sizeof(struct StatementBloc));
    (*stm) = (*stm) -> next;
}

static inline void addStatementSwitch(
    struct StatementBloc**      stm,
    struct StatementSwitchCase* cases
) {
    struct StatementSwitch* res =
        memoryAlloc(sizeof(struct StatementSwitchCase));
    res -> cases = cases;
    (*stm) -> expr = (struct Statement) {
        .type = STATEMENT_SWITCH,
        .statement_switch = res
    };
    (*stm) -> next = memoryAlloc(sizeof(struct StatementBloc));
    (*stm) = (*stm) -> next;
}

static inline void addStatementIf(
    struct StatementBloc** stm,
    struct Expretion*      condition,
    struct StatementBloc*  then
) {
    struct StatementIf* res = memoryAlloc(sizeof(struct StatementIf));
    res -> condition = condition;
    res -> then = then;
    (*stm) -> expr = (struct Statement) {
        .type = STATEMENT_DO,
        .statement_if = res
    };
    (*stm) -> next = memoryAlloc(sizeof(struct StatementBloc));
    (*stm) = (*stm) -> next;
}

static inline void addStatementDo(
    struct StatementBloc** stm,
    bool                   is_label,
    struct String          label,
    struct Expretion*      condition,
    struct StatementBloc*  then
) {
    struct StatementDo* res = memoryAlloc(sizeof(struct StatementDo));
    res -> is_label = is_label;
    res -> label = label;
    res -> condition = condition;
    res -> then = then;
    (*stm) -> expr = (struct Statement) {
        .type = STATEMENT_DO,
        .statement_do = res
    };
    (*stm) -> next = memoryAlloc(sizeof(struct StatementBloc));
    (*stm) = (*stm) -> next;
}

static inline void addStatementWhile(
    struct StatementBloc** stm,
    bool                   is_label,
    struct String          label,
    struct Expretion*      condition,
    struct StatementBloc*  then
) {
    struct StatementWhile* res = memoryAlloc(sizeof(struct StatementWhile));
    res -> is_label = is_label;
    res -> label = label;
    res -> condition = condition;
    res -> then = then;
    (*stm) -> expr = (struct Statement) {
        .type = STATEMENT_WHILE,
        .statement_while = res
    };
    (*stm) -> next = memoryAlloc(sizeof(struct StatementBloc));
    (*stm) = (*stm) -> next;
}

static inline void addStatementFor(
    struct StatementBloc** stm,
    bool                  is_label,
    struct String         label,
    struct String         var,
    struct Expretion*     condition,
    struct StatementBloc* then
) {
    struct StatementFor* res = memoryAlloc(sizeof(struct StatementFor));
    res -> is_label = is_label;
    res -> label = label;
    res -> var = var;
    res -> condition = condition;
    res -> then = then;
    (*stm) -> expr = (struct Statement) {
        .type = STATEMENT_FOR,
        .statement_for = res
    };
    (*stm) -> next = memoryAlloc(sizeof(struct StatementBloc));
    (*stm) = (*stm) -> next;
}

static inline void addStatementRepead(
    struct StatementBloc** stm,
    bool                   is_label,
    struct String          label,
    struct Expretion*      condition,
    struct StatementBloc*  then
) {
    struct StatementRepead* res = memoryAlloc(sizeof(struct StatementRepead));
    res -> is_label = is_label;
    res -> label = label;
    res -> condition = condition;
    res -> then = then;
    (*stm) -> expr = (struct Statement) {
        .type = STATEMENT_REPEAD,
        .statement_repead = res
    };
    (*stm) -> next = memoryAlloc(sizeof(struct StatementBloc));
    (*stm) = (*stm) -> next;
}

static inline void addStatementReturn(
    struct StatementBloc** stm,
    struct Expretion*      value
) {
    struct StatementReturn* res = memoryAlloc(sizeof(struct StatementReturn));
    res -> value = value;
    (*stm) -> expr = (struct Statement) {
        .type = STATEMENT_RETURN,
        .statement_return = res
    };
    (*stm) -> next = memoryAlloc(sizeof(struct StatementBloc));
    (*stm) = (*stm) -> next;
}

static inline void addStatementAsign(
    struct StatementBloc** stm,
    struct Expretion*      get_expr,
    struct String          var_name,
    struct Expretion*      value
) {
    struct StatementAsign* res = memoryAlloc(sizeof(struct StatementAsign));
    res -> get_expr = get_expr;
    res -> var_name = var_name;
    res -> value = value;
    (*stm) -> expr = (struct Statement) {
        .type = STATEMENT_ASIGN,
        .statement_asign = res
    };
    (*stm) -> next = memoryAlloc(sizeof(struct StatementBloc));
    (*stm) = (*stm) -> next;
}

static inline struct Expretion* literalSting(struct String string) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_LOGICAL_OR;
    res -> literal = (struct Literal) {
        .type = LITERAL_STING,
        .string = string
    };
    return res;
}

static inline struct Expretion* literalFloat(long double _float) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_LOGICAL_OR;
    res -> literal = (struct Literal) {
        .type = LITERAL_FLOAT,
        ._float = _float
    };
    return res;
}

static inline struct Expretion* literalInt(uint64_t _int) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_LOGICAL_OR;
    res -> literal = (struct Literal) {
        .type = LITERAL_INT,
        ._int = _int
    };
    return res;
}

static inline struct Expretion* literalName(struct Path* name) {
    struct Expretion* res = memoryAlloc(sizeof(struct Expretion));
    res -> type = EXPRETION_LOGICAL_OR;
    res -> literal = (struct Literal) {
        .type = LITERAL_NAME,
        .name = name
    };
    return res;
}

static inline void FunctionArgs(
    struct FunctionArgs** args,
    struct Expretion      arg
) {
    (*args) -> arg = arg;
    (*args) -> next = memoryAlloc(sizeof(struct FunctionArgs));
    (*args) = (*args) -> next;
}

static inline void appendEnumFildTyped(
    struct EnumFildList** _enum,
    struct TypeFild       fild
) {
    (*_enum) -> type = ENUM_FILD_TYPED;
    (*_enum) -> typed = fild;
    (*_enum) -> next = memoryAlloc(sizeof(struct EnumFildList));
    (*_enum) = (*_enum) -> next;
}

static inline void appendEnumFildUntyped(
    struct EnumFildList** _enum,
    struct String         fild
) {
    (*_enum) -> type = ENUM_FILD_UNTYPED;
    (*_enum) -> untyped = fild;
    (*_enum) -> next = memoryAlloc(sizeof(struct EnumFildList));
    (*_enum) = (*_enum) -> next;
}

static inline void appendTypeFildList(
    struct TypeFildList** filds,
    struct TypeFild       type
) {
    (*filds) -> type = type;
    (*filds) -> next = memoryAlloc(sizeof(struct TypeFildList));
    (*filds) = (*filds) -> next;
}

static inline void  appendTypeArgs(
    struct TypeArgs** args,
    struct Type       type
) {
    (*args) -> type = type;
    (*args) -> next = memoryAlloc(sizeof(struct TypeArgs));
    (*args) = (*args) -> next;
}

static inline void appendTypeParams(
    struct TypeParams** params,
    struct String       name
) {
    (*params) -> name = name;
    (*params) -> next = memoryAlloc(sizeof(struct TypeParams));
    (*params) = (*params) -> next;
}

static inline void appendPath(struct Path** path, struct String name) {
    (*path) -> name = name;
    (*path) -> next = memoryAlloc(sizeof(struct Path));
    (*path) = (*path) -> next;
}

static inline void addImport(struct AST** ast, struct Import import) {
    (*ast) -> type = AST_IMPORT;
    (*ast) -> ast_import = import;
    (*ast) -> next = memoryAlloc(sizeof(struct AST));
    (*ast) = (*ast) -> next;
}

static inline void addTest(struct AST** ast, struct Test test) {
    (*ast) -> type = AST_TYPE;
    (*ast) -> ast_test = test;
    (*ast) -> next = memoryAlloc(sizeof(struct AST));
    (*ast) = (*ast) -> next;
}

static inline void addType(struct AST** ast, struct TypeDecl type) {
    (*ast) -> type = AST_TYPE;
    (*ast) -> ast_type = type;
    (*ast) -> next = memoryAlloc(sizeof(struct AST));
    (*ast) = (*ast) -> next;
}

static inline void addFunc(struct AST** ast, struct FuncDecl func) {
    (*ast) -> type = AST_FUNC;
    (*ast) -> ast_func = func;
    (*ast) -> next = memoryAlloc(sizeof(struct AST));
    (*ast) = (*ast) -> next;
}

static inline void addCFunc(struct AST** ast, struct CFuncDecl func) {
    (*ast) -> type = AST_CFUNC;
    (*ast) -> ast_cfunc = func;
    (*ast) -> next = memoryAlloc(sizeof(struct AST));
    (*ast) = (*ast) -> next;
}

#endif
