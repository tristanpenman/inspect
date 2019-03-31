%%{

machine formula;

main := |*

('-'?[0-9]+('.'[0-9]+)?)
    {
        cbToken(LITERAL, new LitDoubleNode(atof(getStr(ts, te).c_str())), pData);
    };

("'"[^']*"'") | ('"'[^"]*'"')
    {
        // String literals appear between a pair of ' or " characters. The
        // delimiters are not passed along with the string.
        cbToken(LITERAL, new LitStringNode(getStr(ts+1, te-1)), pData);
    };

([A-Za-z]+[0-9]+)
    {
        // When an identifier looks like it could be address, it is passed to
        // parser using the ADDRESS_OR_IDENTIFIER token. The parser can
        // determine how to treat the token based on its context.
        cbToken(ADDRESS_OR_IDENTIFIER, new VarIdentifierNode(getStr(ts, te)), pData);
    };

([A-Za-z][0-9a-zA-Z_]*)
    {
        // The reason the IDENTIFIER token is still used is because
        // identifiers may contain underscores, and do not need to contain
        // numbers. Currently, identifiers may only be used for function
        // names.
        cbToken(IDENTIFIER, new VarIdentifierNode(getStr(ts, te)), pData);
    };

("'" any*)
    {
        // A formula that begins with an apostrophe should be interpreted
        // as a literal string. This is shorthand that allows numbers to
        // be entered as a text value.
        cbToken(LITERAL, new LitStringNode(getStr(ts + 1, te)), pData);
    };

','
    {
        cbToken(COMMA, NULL, pData);
    };

'='
    {
        cbToken(EQUALS, NULL, pData);
    };

('+')
    {
        cbToken(PLUS, NULL, pData);
    };

('-')
    {
        cbToken(MINUS, NULL, pData);
    };

('*')
    {
        cbToken(TIMES, NULL, pData);
    };

"("
    {
        cbToken(LPAREN, NULL, pData);
    };

")"
    {
        cbToken(RPAREN, NULL, pData);
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

#include <stdexcept>

#include "ast.hpp"
#include "formula.hpp"
#include "parser.h"
#include "util.hpp"

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
        void * pParser;
        ParserData * pParserData;
    };

    typedef void (*CallbackToken)(int kind, Node * pNode, CallbackData * pData);
    typedef void (*CallbackEnd)(CallbackData * pData);

    void cbToken(int kind, Node * pNode, CallbackData * pData)
    {
        Parse(pData->pParser, kind, pNode, pData->pParserData);
    }

    void cbEnd(CallbackData * pData)
    {
        Parse(pData->pParser, 0, 0, pData->pParserData);
    }

    Node * addressNodeFromIdentifierNode(const Node * pNode)
    {
        const VarIdentifierNode * pIdentifierNode = dynamic_cast<const VarIdentifierNode *>(pNode);
        if (!pIdentifierNode) {
            throw std::runtime_error("Source is not an identifier node.");
        }

        return new VarAddressNode(Address(pIdentifierNode->getName()));
    }

    Node * beginFunctionCallNode(const Node * pNode)
    {
        FnCallNode * pFnCallNode = new FnCallNode();
        pFnCallNode->pushParam(pNode);
        return pFnCallNode;
    }

    Node * createBinaryOpNode(BinaryOp binaryOp, const Node * left, const Node * right)
    {
        return new BinaryOpNode(binaryOp, left, right);
    }

    void deleteNode(Node * pNode)
    {
        delete pNode;
    }

    void endFunctionCallNode(Node * pTargetNode, const Node * pSourceNode)
    {
        const VarIdentifierNode * pIdentifierNode = dynamic_cast<const VarIdentifierNode *>(pSourceNode);
        if (!pIdentifierNode) {
            throw std::runtime_error("Source is not an identifier node [endFunctionCallNode].");
        }

        FnCallNode * pFnCallNode = dynamic_cast<FnCallNode *>(pTargetNode);
        if (!pFnCallNode) {
            throw std::runtime_error("Target is not a function call node [endFunctionCallNode].");
        }

        pFnCallNode->setFnName(pIdentifierNode->getName());
    }

    void extendFunctionCallNode(Node * pTargetNode, const Node * pSourceNode)
    {
        FnCallNode * pFnCallNode = dynamic_cast<FnCallNode *>(pTargetNode);
        if (!pFnCallNode) {
            throw std::runtime_error("Target is not a function call node [extendFunctionCallNode].");
        }

        pFnCallNode->pushParam(pSourceNode);
    }
}

Formula::Formula(const std::string & formula)
{
    // Initialise the Parser and ParserData structures that will be passed
    // in to the callback functions below.
    void * pParser = ParseAlloc(::operator new);

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
        pParser,
        &parserData
    };

    CallbackData *pData = &data;

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

    cbEnd(pData);

    ParseFree(pParser, ::operator delete);

    if (parserData.hadStackOverflow) {
        delete parserData.pRoot;
        throw std::runtime_error("Stack overflow.");
    } else if (parserData.hadError || unmatched) {
        delete parserData.pRoot;
        throw std::runtime_error("Invalid formula.");
    } else if (parserData.pRoot) {
        m_pRoot = std::shared_ptr<Node>(parserData.pRoot);
    } else {
        throw std::runtime_error("Internal error.");
    }
}

std::string Formula::evaluate(EvalAddressCallback evalAddrCb, EvalFunctionCallback evalFuncCb, void *pData)
{
    return m_pRoot->evaluate(evalAddrCb, evalFuncCb, pData);
}

Formula::operator std::string() const
{
    return *m_pRoot;
}
