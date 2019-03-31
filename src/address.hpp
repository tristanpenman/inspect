#pragma once

#include <string>

struct Address
{
    /**
     * Construct an Address using strongly typed column and row values.
     *
     * @param   column  Column value
     * @param   row     Row value
     */
    Address(unsigned int column, unsigned int row);

    /*
     * Construct an Address using an address in string format.
     *
     * @param   address  Address in string format
     */
    Address(const std::string & address);

    /// Column offset (beginning at 0)
    unsigned int column;

    /// Row offset (beginning at 0)
    unsigned int row;
};

bool operator<(const Address & lhs, const Address & rhs);

bool operator==(const Address & lhs, const Address & rhs);
