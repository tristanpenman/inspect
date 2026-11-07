#pragma once

#include <memory>
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
    virtual ~Node() = default;
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
    double value_;
};

class LitStringNode : public Node
{
public:
    LitStringNode(const std::string & value);
    virtual std::string evaluate(EvalAddressCallback, EvalFunctionCallback, void * pData) const;
    virtual operator std::string() const;
private:
    std::string value_;
};

class BinaryOpNode: public Node
{
public:
    BinaryOpNode(BinaryOp binaryOp, std::unique_ptr<Node> left, std::unique_ptr<Node> right);
    virtual std::string evaluate(EvalAddressCallback, EvalFunctionCallback, void * pData) const;
    virtual operator std::string() const;
private:
    BinaryOp binaryOp_;
    std::unique_ptr<Node> left_;
    std::unique_ptr<Node> right_;
};

class VarIdentifierNode: public Node
{
public:
    VarIdentifierNode(const std::string & name);
    const std::string & getName() const;
    virtual std::string evaluate(EvalAddressCallback, EvalFunctionCallback, void * pData) const;
    virtual operator std::string() const;
private:
    std::string name_;
};

class VarAddressNode: public Node
{
public:
    VarAddressNode(const Address & address);
    const Address & getAddress() const;
    virtual std::string evaluate(EvalAddressCallback, EvalFunctionCallback, void * pData) const;
    virtual operator std::string() const;
private:
    Address address_;
};

class FnCallNode: public Node
{
public:
    virtual ~FnCallNode();
    void setFnName(const std::string& functionName);
    void pushParam(std::unique_ptr<Node> node);
    virtual std::string evaluate(EvalAddressCallback, EvalFunctionCallback, void * pData) const;
    virtual operator std::string() const;
private:
    typedef std::vector<std::unique_ptr<Node>> Params;
    Params params_;
    std::string functionName_;
};
