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

    Formula(const std::string& formula);
    ~Formula();

    std::string evaluate(EvalAddressCallback, EvalFunctionCallback, void* data) const;

    operator std::string() const;

private:

    std::unique_ptr<Node> root_;
};
