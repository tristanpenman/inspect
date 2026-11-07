#include "ast.hpp"

#include <sstream>
#include <utility>

namespace {

template<typename ValueType>
std::string toString(ValueType value)
{
    std::stringstream stream;
    stream << value;
    return stream.str();
}

}  // namespace

LitDoubleNode::LitDoubleNode(double value)
    : value_(value)
{
}

std::string LitDoubleNode::evaluate(
    EvalAddressCallback,
    EvalFunctionCallback,
    void*) const
{
    return toString(value_);
}

LitDoubleNode::operator std::string() const
{
    return toString(value_);
}

LitStringNode::LitStringNode(const std::string& value)
    : value_(value)
{
}

std::string LitStringNode::evaluate(
    EvalAddressCallback,
    EvalFunctionCallback,
    void*) const
{
    return value_;
}

LitStringNode::operator std::string() const
{
    std::stringstream stream;
    stream << "str{" << value_ << "}";
    return stream.str();
}

BinaryOpNode::BinaryOpNode(
    BinaryOp binaryOp,
    std::unique_ptr<Node> left,
    std::unique_ptr<Node> right)
    : binaryOp_(binaryOp)
    , left_(std::move(left))
    , right_(std::move(right))
{
}

std::string BinaryOpNode::evaluate(
    EvalAddressCallback evalAddress,
    EvalFunctionCallback evalFunction,
    void* data) const
{
    const std::string valueLeft = left_->evaluate(evalAddress, evalFunction, data);
    const std::string valueRight = right_->evaluate(evalAddress, evalFunction, data);

    std::stringstream stream(valueLeft);
    double numberLeft = 0;
    stream >> numberLeft;
    if (!stream.fail()) {
        stream.str("");
        stream.clear();
        stream << valueRight;
        double numberRight = 0;
        stream >> numberRight;
        if (!stream.fail()) {
            switch (binaryOp_) {
                case BINARY_OP_ADD:
                    return toString(numberLeft + numberRight);
                case BINARY_OP_SUBTRACT:
                    return toString(numberLeft - numberRight);
                case BINARY_OP_MULTIPLY:
                    return toString(numberLeft * numberRight);
                case BINARY_OP_DIVIDE:
                    return toString(numberLeft / numberRight);
            }
        }
    }

    switch (binaryOp_) {
        case BINARY_OP_ADD:
            return valueLeft + valueRight;
        default:
            break;
    }

    return "ERROR";
}

BinaryOpNode::operator std::string() const
{
    std::stringstream stream;
    stream << "(" << std::string(*left_);

    switch (binaryOp_) {
        case BINARY_OP_ADD: stream << " + "; break;
        case BINARY_OP_SUBTRACT: stream << " - "; break;
        case BINARY_OP_MULTIPLY: stream << " * "; break;
        case BINARY_OP_DIVIDE: stream << " / "; break;
        default: stream << " ? "; break;
    }

    stream << std::string(*right_) << ")";
    return stream.str();
}

VarIdentifierNode::VarIdentifierNode(const std::string& name)
    : name_(name)
{
}

const std::string& VarIdentifierNode::getName() const
{
    return name_;
}

std::string VarIdentifierNode::evaluate(
    EvalAddressCallback,
    EvalFunctionCallback,
    void*) const
{
    return name_;
}

VarIdentifierNode::operator std::string() const
{
    std::stringstream stream;
    stream << "id{" << name_ << "}";
    return stream.str();
}

VarAddressNode::VarAddressNode(const Address& address)
    : address_(address)
{
}

const Address& VarAddressNode::getAddress() const
{
    return address_;
}

std::string VarAddressNode::evaluate(
    EvalAddressCallback evalAddress,
    EvalFunctionCallback,
    void* data) const
{
    return evalAddress(address_, data);
}

VarAddressNode::operator std::string() const
{
    std::stringstream stream;
    stream << "addr{" << address_.column << "," << address_.row << "}";
    return stream.str();
}

void FnCallNode::setFnName(const std::string& functionName)
{
    functionName_ = functionName;
}

FnCallNode::~FnCallNode() = default;

void FnCallNode::pushParam(std::unique_ptr<Node> node)
{
    params_.push_back(std::move(node));
}

std::string FnCallNode::evaluate(
    EvalAddressCallback evalAddress,
    EvalFunctionCallback evalFunction,
    void* data) const
{
    Arguments arguments;
    for (const auto& param : params_) {
        arguments.push_back(param->evaluate(evalAddress, evalFunction, data));
    }

    return evalFunction(functionName_, arguments, data);
}

FnCallNode::operator std::string() const
{
    std::stringstream stream;
    stream << "fn{" << functionName_ << "}(";
    for (std::size_t index = 0; index < params_.size(); ++index) {
        if (index > 0) {
            stream << ",";
        }
        stream << std::string(*params_[index]);
    }
    stream << ")";
    return stream.str();
}
