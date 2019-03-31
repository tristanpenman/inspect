%%{

machine address;

main := |*

([A-Za-z]+)
    {
        column = 0;
        unsigned int multiplier = 1;
        const std::string s = getStr(ts, te);
        for (std::string::const_iterator c = s.begin(); c != s.end(); c++) {
            if (*c >= 'a' && *c <= 'z') {
                column += (unsigned int)(*c - 'a' + 1) * multiplier;
            } else if (*c >= 'A' && *c <= 'Z') {
                column += (unsigned int)(*c - 'A' + 1) * multiplier;
            }
            multiplier *= 26;
        }

        columnSet = true;
    };

([0-9]+)
    {
        if (columnSet) {
            std::stringstream ss(getStr(ts, te));
            ss >> row;
            rowSet = !ss.fail();
        }
    };

*|;

}%%

#include <sstream>
#include <stdexcept>
#include <iostream>

#include "address.hpp"
#include "util.hpp"

%% write data;

Address::Address(unsigned int column, unsigned int row)
    : column(column)
    , row(row)
{
    // No further initialisation
}

Address::Address(const std::string &address)
    : column(0)
    , row(0)
{
    bool columnSet = false;
    bool rowSet = false;

    int cs;
    const char * ts;
    const char * te;
    int act;

    %% write init;

    // Setup constants for lexical analyzer
    const char * p = address.c_str();
    const char * pe = address.c_str() + address.size();
    const char * eof = pe;

    %% write exec;

    if (!rowSet || !columnSet) {
        throw std::invalid_argument("Invalid address string.");
    }
}

bool operator<(const Address & lhs, const Address & rhs)
{
    return (lhs.column < rhs.column) || (lhs.column == rhs.column && lhs.row < rhs.row);
}

bool operator==(const Address & lhs, const Address & rhs)
{
    return (lhs.column == rhs.column && lhs.row == rhs.row);
}
