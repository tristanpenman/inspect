#pragma once

#include <string>
#include <vector>

#include "address.hpp"
#include "binary_op.h"

typedef std::vector<std::string> Arguments;

typedef std::string (*EvalAddressCallback)(const Address &, void * pData);
typedef std::string (*EvalFunctionCallback)(const std::string & name, const Arguments &, void * pData);

class Node
{
public:
    virtual ~Node() {};
    virtual std::string evaluate(EvalAddressCallback, EvalFunctionCallback, void * pData) const = 0;
    virtual operator std::string() const = 0;
};

class LitDoubleNode: public Node
{
public:
    LitDoubleNode(double value);
    virtual std::string evaluate(EvalAddressCallback, EvalFunctionCallback, void * pData) const;
    virtual operator std::string() const;
private:
    double m_value;
};

class LitStringNode : public Node
{
public:
    LitStringNode(const std::string & value);
    virtual std::string evaluate(EvalAddressCallback, EvalFunctionCallback, void * pData) const;
    virtual operator std::string() const;
private:
    std::string m_value;
};

class BinaryOpNode: public Node
{
public:
    BinaryOpNode(BinaryOp binaryOp, const Node * pLeft, const Node * pRight);
    virtual ~BinaryOpNode();
    virtual std::string evaluate(EvalAddressCallback, EvalFunctionCallback, void * pData) const;
    virtual operator std::string() const;
private:
    BinaryOp m_binaryOp;
    const Node * m_pLeft;
    const Node * m_pRight;
};

class VarIdentifierNode: public Node
{
public:
    VarIdentifierNode(const std::string & name);
    const std::string & getName() const;
    virtual std::string evaluate(EvalAddressCallback, EvalFunctionCallback, void * pData) const;
    virtual operator std::string() const;
private:
    std::string m_name;
};

class VarAddressNode: public Node
{
public:
    VarAddressNode(const Address & address);
    const Address & getAddress() const;
    virtual std::string evaluate(EvalAddressCallback, EvalFunctionCallback, void * pData) const;
    virtual operator std::string() const;
private:
    Address m_address;
};

class FnCallNode: public Node
{
public:
    virtual ~FnCallNode();
    void setFnName(const std::string & fnName);
    void pushParam(const Node * pNode);
    virtual std::string evaluate(EvalAddressCallback, EvalFunctionCallback, void * pData) const;
    virtual operator std::string() const;
private:
    typedef std::vector<const Node *> Params;
    Params m_params;
    std::string m_fnName;
};
