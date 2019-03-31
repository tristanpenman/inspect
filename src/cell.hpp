#pragma once

#include <string>

struct Cell
{
    Cell(const std::string & formula)
        : formula(formula)
        , value()
        , phase(0)
        , processed(false)
    {
        // No further initialisation
    }

    // Literal cell formula
    std::string formula;

    // Cached value
    std::string value;

    // This variable is used to track when this cell was last re-calculated. If the phase value is
    // the same as that for the parent Sheet instance, then the cell has been visited by the
    // current re-calculation pass. If it has the opposite sign, then the cell was last visited in
    // the previous re-calculation pass.
    int phase;

    // Flag to track whether the cell has been processed in the current recalculation pass
    bool processed;
};
