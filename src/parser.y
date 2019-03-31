%include {
#include <assert.h>
#include "parser.h"
}

%left PLUS MINUS.
%left TIMES DIVIDE.
%right EXP NOT.

%token_type { struct Node * }

%extra_argument { struct ParserData * pData }

formula ::= LITERAL(A).
    {
        pData->pRoot = A;
    }

formula ::= EQUALS expr(A).
    {
        pData->pRoot = A;
    }

expr(A) ::= expr(B) PLUS expr(C).
    {
        A = pData->createBinaryOpNode(BINARY_OP_ADD, B, C);
    }

expr(A) ::= expr(B) MINUS expr(C).
    {
        A = pData->createBinaryOpNode(BINARY_OP_SUBTRACT, B, C);
    }

expr(A) ::= expr(B) TIMES expr(C).
    {
        A = pData->createBinaryOpNode(BINARY_OP_MULTIPLY, B, C);
    }

expr(A) ::= expr(B) DIVIDE expr(C).
    {
        A = pData->createBinaryOpNode(BINARY_OP_DIVIDE, B, C);
    }

expr(A) ::= LPAREN expr(B) RPAREN.
    {
        A = B;
    }

expr(A) ::= LITERAL(B).
    {
        A = B;
    }

expr(A) ::= addr_or_identifier(B) LPAREN params(C) RPAREN.
    {
        // The params non-terminal returns a partially complete instance of the FnCallNode class
        A = C;

        // In order to completely define the function call, the function name must be taken from
        // the identifier returned by the addr_or_identifier non-terminal
        pData->endFunctionCallNode(A, B);
        pData->deleteNode(B);
    }

addr_or_identifier(A) ::= ADDRESS_OR_IDENTIFIER(B).
    {
        A = B;
    }

addr_or_identifier(A) ::= IDENTIFIER(B).
    {
        A = B;
    }

params(A) ::= params(B) COMMA expr(C).
    {
        // The params non-terminal recursively populates an instance of the
        // FnCallNode class.
        A = B;

        // Before we can add the next parameter to the function call, it needs
        // to be cast to the FnCallNode* type. The FnCallNode class now owns the
        // pointer C, so we do not need to delete it.
        pData->extendFunctionCallNode(A, C);
    }

params(A) ::= expr(B).
    {
        // Begin populating a new instance of the FnCallNode class
        A = pData->beginFunctionCallNode(B);
    }

expr(A) ::= ADDRESS_OR_IDENTIFIER(B).
    {
        // Since we know that this identifier is also a valid address we can convert it to an address
        A = pData->addressNodeFromIdentifierNode(B);
        pData->deleteNode(B);
        B = 0;
    }

%token_destructor
    {
        pData->deleteNode($$);
    }

%destructor expr
    {
        pData->deleteNode($$);
    }

%parse_accept
    {
        // Do nothing
    }

%stack_overflow
    {
        pData->hadStackOverflow = true;
    }

%parse_failure
    {
        pData->hadError = true;
    }

%syntax_error
    {
        pData->hadError = true;
    }
