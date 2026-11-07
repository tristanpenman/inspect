#pragma once

#include <memory>
#include <string>
#include <vector>

#include "address.hpp"
#include "binary_op.h"

using Arguments = std::vector<std::string>;

using EvalAddressCallback = std::string (*)(const Address& address, void* data);
using EvalFunctionCallback = std::string (*)(
    const std::string& name,
    const Arguments& arguments,
    void* data);

class Node
{
public:
    virtual ~Node() = default;
    virtual std::string evaluate(
        EvalAddressCallback evalAddress,
        EvalFunctionCallback evalFunction,
        void* data) const = 0;
    virtual operator std::string() const = 0;
};

class LitDoubleNode : public Node
{
public:
    LitDoubleNode(double value);
    std::string evaluate(
        EvalAddressCallback evalAddress,
        EvalFunctionCallback evalFunction,
        void* data) const override;
    operator std::string() const override;
private:
    double value_;
};

class LitStringNode : public Node
{
public:
    LitStringNode(const std::string& value);
    std::string evaluate(
        EvalAddressCallback evalAddress,
        EvalFunctionCallback evalFunction,
        void* data) const override;
    operator std::string() const override;
private:
    std::string value_;
};

class BinaryOpNode : public Node
{
public:
    BinaryOpNode(BinaryOp binaryOp, std::unique_ptr<Node> left, std::unique_ptr<Node> right);
    std::string evaluate(
        EvalAddressCallback evalAddress,
        EvalFunctionCallback evalFunction,
        void* data) const override;
    operator std::string() const override;
private:
    BinaryOp binaryOp_;
    std::unique_ptr<Node> left_;
    std::unique_ptr<Node> right_;
};

class VarIdentifierNode : public Node
{
public:
    VarIdentifierNode(const std::string& name);
    const std::string& getName() const;
    std::string evaluate(
        EvalAddressCallback evalAddress,
        EvalFunctionCallback evalFunction,
        void* data) const override;
    operator std::string() const override;
private:
    std::string name_;
};

class VarAddressNode : public Node
{
public:
    VarAddressNode(const Address& address);
    const Address& getAddress() const;
    std::string evaluate(
        EvalAddressCallback evalAddress,
        EvalFunctionCallback evalFunction,
        void* data) const override;
    operator std::string() const override;
private:
    Address address_;
};

class FnCallNode : public Node
{
public:
    ~FnCallNode() override;
    void setFnName(const std::string& functionName);
    void pushParam(std::unique_ptr<Node> node);
    std::string evaluate(
        EvalAddressCallback evalAddress,
        EvalFunctionCallback evalFunction,
        void* data) const override;
    operator std::string() const override;
private:
    typedef std::vector<std::unique_ptr<Node>> Params;
    Params params_;
    std::string functionName_;
};
