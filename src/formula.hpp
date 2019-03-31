#pragma once

#include <memory>
#include <string>
#include <vector>

#include "address.hpp"

class Node;

class Formula
{
public:
    typedef std::vector<std::string> Arguments;

    typedef std::string (*EvalAddressCallback)(const Address &, void * pData);
    typedef std::string (*EvalFunctionCallback)(const std::string & name, const Arguments &, void * pData);

    Formula(const std::string &);

    std::string evaluate(EvalAddressCallback, EvalFunctionCallback, void * pData);

    operator std::string() const;

private:

    std::shared_ptr<Node> m_pRoot;
};
