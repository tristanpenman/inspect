#include "sheet.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

#include "address.hpp"
#include "cell.hpp"
#include "formula.hpp"

namespace {

struct SheetCallbackData
{
    Cells& cells;
    int phase;
};

void recalculateDepthFirst(int phase, Cells& cells, Cell& cell);

std::string evalAddressCallback(const Address& address, void* data)
{
    SheetCallbackData* callbackData = static_cast<SheetCallbackData*>(data);
    Cells::iterator cell = callbackData->cells.find(address);
    if (cell == callbackData->cells.end()) {
        return "";
    }

    recalculateDepthFirst(callbackData->phase, callbackData->cells, cell->second);
    return cell->second.value;
}

std::string evalFunctionCallback(const std::string&, const Formula::Arguments&, void*)
{
    throw std::runtime_error("Function calls are not implemented.");
}

void recalculateDepthFirst(int phase, Cells& cells, Cell& cell)
{
    // Check if the cell has been discovered in this recalculation phase.
    if (cell.phase == phase) {
        // A processed cell is already recalculated; otherwise this is a cycle.
        if (cell.processed) {
            return;
        }
        throw std::runtime_error("Cycle detected.");
    }

    SheetCallbackData callbackData = {cells, phase};

    cell.phase = phase;
    cell.processed = false;
    cell.value = Formula(cell.formula).evaluate(
        evalAddressCallback,
        evalFunctionCallback,
        &callbackData);
    cell.processed = true;
}

}  // namespace

Sheet::Sheet()
    : phase_(1)
{
}

bool Sheet::erase(const Address& address)
{
    return cells_.erase(address) == 1;
}

std::string Sheet::getFormula(const Address& address) const
{
    const auto cell = cells_.find(address);
    if (cell != cells_.end()) {
        return cell->second.formula;
    }

    return "";
}

std::string Sheet::getValue(const Address& address) const
{
    const auto cell = cells_.find(address);
    if (cell != cells_.end()) {
        return cell->second.value;
    }

    return "";
}

void Sheet::print() const
{
    for (const auto& cell : cells_) {
        std::cout << "[" << cell.first.column << "," << cell.first.row << "]: "
                  << cell.second.value << std::endl;
    }
}

void Sheet::recalculate()
{
    phase_ *= -1;

    for (auto& cell : cells_) {
        recalculateDepthFirst(phase_, cells_, cell.second);
    }
}

bool Sheet::setFormula(const Address& address, const std::string& formula)
{
    auto cell = cells_.find(address);
    if (cell == cells_.end()) {
        return cells_.insert(Cells::value_type(address, Cell(formula))).second;
    }

    cell->second.formula = formula;
    return true;
}
