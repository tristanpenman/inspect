%%{

machine formula;

main := |*

('-'?[0-9]+('.'[0-9]+)?)
    {
        cbToken(LITERAL, new LitDoubleNode(atof(getStr(ts, te).c_str())), callbackData);
    };

("'"[^']*"'") | ('"'[^"]*'"')
    {
        // String literals appear between a pair of ' or " characters. The
        // delimiters are not passed along with the string.
        cbToken(LITERAL, new LitStringNode(getStr(ts+1, te-1)), callbackData);
    };

([A-Za-z]+[0-9]+)
    {
        // When an identifier looks like it could be address, it is passed to
        // parser using the ADDRESS_OR_IDENTIFIER token. The parser can
        // determine how to treat the token based on its context.
        cbToken(ADDRESS_OR_IDENTIFIER, new VarIdentifierNode(getStr(ts, te)), callbackData);
    };

([A-Za-z][0-9a-zA-Z_]*)
    {
        // The reason the IDENTIFIER token is still used is because
        // identifiers may contain underscores, and do not need to contain
        // numbers. Currently, identifiers may only be used for function
        // names.
        cbToken(IDENTIFIER, new VarIdentifierNode(getStr(ts, te)), callbackData);
    };

("'" any*)
    {
        // A formula that begins with an apostrophe should be interpreted
        // as a literal string. This is shorthand that allows numbers to
        // be entered as a text value.
        cbToken(LITERAL, new LitStringNode(getStr(ts + 1, te)), callbackData);
    };

','
    {
        cbToken(COMMA, NULL, callbackData);
    };

'='
    {
        cbToken(EQUALS, NULL, callbackData);
    };

('+')
    {
        cbToken(PLUS, NULL, callbackData);
    };

('-')
    {
        cbToken(MINUS, NULL, callbackData);
    };

('*')
    {
        cbToken(TIMES, NULL, callbackData);
    };

"("
    {
        cbToken(LPAREN, NULL, callbackData);
    };

")"
    {
        cbToken(RPAREN, NULL, callbackData);
    };

space
    {
        // Ignore whitespace
    };

any
    {
        unmatched = true;
    };

*|;

}%%

#include <memory>
#include <stdexcept>
#include <utility>

#include "ast.h"
#include "formula.h"
#include "parser.h"
#include "util.h"

%% write data;

extern "C"
{
    void Parse(
        void * pParser,                    /** The parser */
        int kind,                          /** The major token code number */
        Node * pNode,                      /** The value for the token */
        ParserData * pParserdata           /** Optional %extra_argument parameter */
    );

    void *ParseAlloc(
        void * (*mallocProc)(size_t)       /** Function used to allocate memory */
    );

    void ParseFree(
        void * pParser,                    /** The parser to be deleted */
        void (*freeProc)(void*)            /** Function used to reclaim memory */
    );
}

namespace
{
    struct CallbackData
    {
        void* parser;
        ParserData* parserData;
    };

    typedef void (*CallbackToken)(int kind, Node* node, CallbackData* data);
    typedef void (*CallbackEnd)(CallbackData* data);

    void cbToken(int kind, Node* node, CallbackData* data)
    {
        Parse(data->parser, kind, node, data->parserData);
    }

    void cbEnd(CallbackData* data)
    {
        Parse(data->parser, 0, 0, data->parserData);
    }

    Node* addressNodeFromIdentifierNode(const Node* node)
    {
        const VarIdentifierNode* identifierNode = dynamic_cast<const VarIdentifierNode*>(node);
        if (!identifierNode) {
            throw std::runtime_error("Source is not an identifier node.");
        }

        return new VarAddressNode(Address(identifierNode->getName()));
    }

    Node* beginFunctionCallNode(Node* node)
    {
        FnCallNode* functionCall = new FnCallNode();
        functionCall->pushParam(std::unique_ptr<Node>(node));
        return functionCall;
    }

    Node* createBinaryOpNode(BinaryOp binaryOp, Node* left, Node* right)
    {
        return new BinaryOpNode(
            binaryOp,
            std::unique_ptr<Node>(left),
            std::unique_ptr<Node>(right));
    }

    void deleteNode(Node* node)
    {
        delete node;
    }

    void endFunctionCallNode(Node* targetNode, const Node* sourceNode)
    {
        const VarIdentifierNode* identifierNode = dynamic_cast<const VarIdentifierNode*>(sourceNode);
        if (!identifierNode) {
            throw std::runtime_error("Source is not an identifier node [endFunctionCallNode].");
        }

        FnCallNode* functionCall = dynamic_cast<FnCallNode*>(targetNode);
        if (!functionCall) {
            throw std::runtime_error("Target is not a function call node [endFunctionCallNode].");
        }

        functionCall->setFnName(identifierNode->getName());
    }

    void extendFunctionCallNode(Node* targetNode, Node* sourceNode)
    {
        FnCallNode* functionCall = dynamic_cast<FnCallNode*>(targetNode);
        if (!functionCall) {
            throw std::runtime_error("Target is not a function call node [extendFunctionCallNode].");
        }

        functionCall->pushParam(std::unique_ptr<Node>(sourceNode));
    }
}

Formula::Formula(const std::string& formula)
{
    // Initialise the Parser and ParserData structures that will be passed
    // in to the callback functions below.
    void* parser = ParseAlloc(::operator new);

    ParserData parserData = {
        addressNodeFromIdentifierNode,
        beginFunctionCallNode,
        createBinaryOpNode,
        deleteNode,
        endFunctionCallNode,
        extendFunctionCallNode,
        nullptr,
        false,
        false
    };

    CallbackData data = {
        parser,
        &parserData
    };

    CallbackData* callbackData = &data;

    // Begin tokenising the formula string. As tokens are identified, they
    // will be passed to the Parser via the CallbackToken function. Once all
    // tokens have been identified, the CallbackEnd function is called to
    // indicate to the Parser that there are no more tokens.

    int cs;
    const char * ts;
    const char * te;
    int act;

    bool unmatched = false;

    %% write init;

    // Setup constants for lexical analyzer
    const char * p = formula.c_str();
    const char * pe = formula.c_str() + formula.size();
    const char * eof = pe;

    // Embed lexical analyzer
    %% write exec;

    cbEnd(callbackData);

    ParseFree(parser, ::operator delete);

    std::unique_ptr<Node> root(parserData.root);
    parserData.root = nullptr;

    if (parserData.hadStackOverflow) {
        throw std::runtime_error("Stack overflow.");
    } else if (parserData.hadError || unmatched) {
        throw std::runtime_error("Invalid formula.");
    } else if (root) {
        root_ = std::move(root);
    } else {
        throw std::runtime_error("Internal error.");
    }
}

Formula::~Formula() = default;

std::string Formula::evaluate(
    EvalAddressCallback evalAddress,
    EvalFunctionCallback evalFunction,
    void* data) const
{
    return root_->evaluate(evalAddress, evalFunction, data);
}

Formula::operator std::string() const
{
    return *root_;
}
