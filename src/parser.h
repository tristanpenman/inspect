#pragma once

#include <stdbool.h>

#include "binary_op.h"

#define PLUS                            1
#define MINUS                           2
#define TIMES                           3
#define DIVIDE                          4
#define EXP                             5
#define NOT                             6
#define LITERAL                         7
#define EQUALS                          8
#define LPAREN                          9
#define RPAREN                         10
#define ADDRESS_OR_IDENTIFIER          11
#define IDENTIFIER                     12
#define COMMA                          13

struct Node;

typedef struct Node * (*AddressNodeFromIdentifierNode)(const struct Node *);
typedef struct Node * (*BeginFunctionCallNode)(const struct Node *);
typedef struct Node * (*CreateBinaryOpNode)(enum BinaryOp, const struct Node *, const struct Node *);
typedef void (*DeleteNode)(struct Node *);
typedef void (*EndFunctionCallNode)(struct Node *, const struct Node *);
typedef void (*ExtendFunctionCallNode)(struct Node *, const struct Node *);

struct ParserData
{
    AddressNodeFromIdentifierNode addressNodeFromIdentifierNode;
    BeginFunctionCallNode beginFunctionCallNode;
    CreateBinaryOpNode createBinaryOpNode;
    DeleteNode deleteNode;
    EndFunctionCallNode endFunctionCallNode;
    ExtendFunctionCallNode extendFunctionCallNode;

    struct Node * pRoot;

    bool hadError;
    bool hadStackOverflow;
};
