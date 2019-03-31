#include <sstream>

#include "ast.hpp"

namespace {
    template<typename T>
    std::string toString(T t) {
        std::stringstream ss;
        ss << t;
        return ss.str();
    }
}

// ----------------------------------------------------------------------------
//
// LitDoubleNode
//
// ----------------------------------------------------------------------------

LitDoubleNode::LitDoubleNode(double value)
    : m_value(value)
{
    // No further initialisation
}

std::string LitDoubleNode::evaluate(EvalAddressCallback evalAddrCb, EvalFunctionCallback evalFuncCb, void * pData) const
{
    std::ostringstream ss;
    ss << m_value;
    return ss.str();
}

LitDoubleNode::operator std::string() const
{
    std::stringstream ss;
    ss << m_value;
    return ss.str();
}

// ----------------------------------------------------------------------------
//
// LitStringNode
//
// ----------------------------------------------------------------------------

LitStringNode::LitStringNode(const std::string & value)
    : m_value(value)
{
    // No further initialisation
}

std::string LitStringNode::evaluate(EvalAddressCallback evalAddrCb, EvalFunctionCallback evalFuncCb, void * pData) const
{
    return m_value;
}

LitStringNode::operator std::string() const
{
    std::stringstream ss;
    ss << "str{" << m_value << "}";
    return ss.str();
}

// ----------------------------------------------------------------------------
//
// BinaryOpNode
//
// ----------------------------------------------------------------------------

BinaryOpNode::BinaryOpNode(BinaryOp binaryOp, const Node * pLeft, const Node * pRight)
    : m_binaryOp(binaryOp)
    , m_pLeft(pLeft)
    , m_pRight(pRight)
{
    // No further initialisation
}

BinaryOpNode::~BinaryOpNode()
{
    delete m_pLeft;
    m_pLeft = 0;
    delete m_pRight;
    m_pRight = 0;
}

std::string BinaryOpNode::evaluate(EvalAddressCallback evalAddrCb, EvalFunctionCallback evalFuncCb, void * pData) const {
    const std::string valueLeft = m_pLeft->evaluate(evalAddrCb, evalFuncCb, pData);
    const std::string valueRight = m_pRight->evaluate(evalAddrCb, evalFuncCb, pData);

    std::stringstream ss(valueLeft);
    double dLeft = 0;
    ss >> dLeft;
    if (!ss.fail()) {
        ss.str("");
        ss.clear();
        ss << valueRight;
        double dRight = 0;
        ss >> dRight;
        if (!ss.fail()) {
            switch (m_binaryOp) {
                case BINARY_OP_ADD:
                    return toString(dLeft + dRight);
                case BINARY_OP_SUBTRACT:
                    return toString(dLeft - dRight);
                case BINARY_OP_MULTIPLY:
                    return toString(dLeft * dRight);
                case BINARY_OP_DIVIDE:
                    return toString(dLeft / dRight);
            }
        }
    }

    switch (m_binaryOp) {
        case BINARY_OP_ADD:
            return std::string(valueLeft).append(valueRight);
        default:
            break;
    }

    return "ERROR";
}

BinaryOpNode::operator std::string() const
{
    std::stringstream ss;
    ss << "(" << std::string(*m_pLeft);

    switch (m_binaryOp) {
        case BINARY_OP_ADD: ss << " + "; break;
        case BINARY_OP_SUBTRACT: ss << " - "; break;
        case BINARY_OP_MULTIPLY: ss << " * "; break;
        case BINARY_OP_DIVIDE: ss << " / "; break;
        default: ss << " ? "; break;
    }

    ss << std::string(*m_pRight) << ")";
    return ss.str();
}

// ----------------------------------------------------------------------------
//
// VarIdentifierNode
//
// ----------------------------------------------------------------------------

VarIdentifierNode::VarIdentifierNode(const std::string & name)
    : m_name(name)
{
    // No further initialisation
}

const std::string & VarIdentifierNode::getName() const
{
    return m_name;
}

std::string VarIdentifierNode::evaluate(EvalAddressCallback evalAddrCb, EvalFunctionCallback evalFuncCb, void * pData) const
{
    return m_name;
}

VarIdentifierNode::operator std::string() const
{
    std::stringstream ss;
    ss << "id{" << m_name << "}";
    return ss.str();
}

// ----------------------------------------------------------------------------
//
// VarAddressNode
//
// ----------------------------------------------------------------------------

VarAddressNode::VarAddressNode(const Address & address)
    : m_address(address)
{

}

const Address& VarAddressNode::getAddress() const
{
    return m_address;
}

std::string VarAddressNode::evaluate(EvalAddressCallback evalAddrCb, EvalFunctionCallback evalFuncCb, void * pData) const
{
    return evalAddrCb(m_address, pData);
}

VarAddressNode::operator std::string() const
{
    std::stringstream ss;
    ss << "addr{" << m_address.column << "," << m_address.row << "}";
    return ss.str();
}

// ----------------------------------------------------------------------------
//
// FnCallNode
//
// ----------------------------------------------------------------------------

FnCallNode::~FnCallNode()
{
    for (Params::iterator itr = m_params.begin(); itr != m_params.end(); itr++) {
        delete *itr;
    }

    m_params.clear();
}

void FnCallNode::setFnName(const std::string & name)
{
    m_fnName = name;
}

void FnCallNode::pushParam(const Node * pNode)
{
    m_params.push_back(pNode);
}

std::string FnCallNode::evaluate(EvalAddressCallback evalAddrCb, EvalFunctionCallback evalFuncCb, void * pData) const
{
    Arguments arguments;
    for (Params::const_iterator itr = m_params.begin(); itr != m_params.end(); itr++) {
        arguments.push_back((*itr)->evaluate(evalAddrCb, evalFuncCb, pData));
    }

    return evalFuncCb(m_fnName, arguments, pData);
}

FnCallNode::operator std::string() const
{
    std::stringstream ss;
    ss << "fn{" << m_fnName << "}(";
    for (Params::const_iterator itr = m_params.begin(); itr != m_params.end(); itr++) {
        if (itr != m_params.begin()) {
            ss << ",";
        }
        ss << std::string(**itr);
    }
    ss << ")";
    return ss.str();
}
