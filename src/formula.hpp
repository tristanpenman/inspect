#pragma once

#include <memory>
#include <string>
#include <vector>

#include "address.hpp"

class Node;

class Formula
{
public:
    using Arguments = std::vector<std::string>;

    using EvalAddressCallback = std::string (*)(const Address&, void* data);
    using EvalFunctionCallback = std::string (*)(
        const std::string& name,
        const Arguments& arguments,
        void* data);

    Formula(const std::string& formula);
    ~Formula();

    std::string evaluate(
        EvalAddressCallback evalAddress,
        EvalFunctionCallback evalFunction,
        void* data) const;

    operator std::string() const;

private:
    std::unique_ptr<Node> root_;
};
