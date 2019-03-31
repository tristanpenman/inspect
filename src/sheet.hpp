#pragma once

#include <map>
#include <memory>
#include <string>

struct Address;
struct Cell;

typedef std::map<Address, Cell> Cells;

class Sheet
{
public:
    Sheet();

    ~Sheet();

    /**
     * Erase the formula for a Cell, identified by an address string.
     *
     * @param   address  Address of Cell to erase
     *
     * @returns true if the Cell was previously set, false otherwise
     */
    bool erase(const Address &);

    /**
     * Retrieve the formula for a Cell identified by an address string, in
     * string format.
     *
     * If the Cell has not been set, then this function will return an empty
     * string.
     *
     * @param   address  Address of cell to query
     *
     * @returns a string containing the formula for a Cell.
     */
    std::string getFormula(const Address &) const;

    /**
     * Retrieve the value of a Cell, identified by an address string, in string
     * format.
     *
     * If the Cell has not been set, then this function will return an empty
     * string.
     *
     * @param   address  Address of the cell to query
     *
     * @returns a string containing the value of the Cell
     */
    std::string getValue(const Address &) const;

    /**
     * Query a Cell, identified by an Address object, to see if it has been set.
     *
     * @param   address  Address of the cell to query
     *
     * @returns true if the Cell has been set, false otherwise.
     */
    bool isSet(const Address & address) const;

    /**
     * Print values of all cells
     */
    void print() const;

    /**
     * Force a recalculation of all values in the sheet.
     *
     * Values for cells are cached in a sparse array of Cell objects, but these
     * values are not updated until this method is invoked on the Sheet.
     */
    void recalculate();

    /**
     * Set the formula for a cell identified by an Address object.
     *
     * @param   address  Address of cell to be updated
     * @param   formula  Formula, in string format
     *
     * @returns true if cell updated successfully, false otherwise
     */
    bool setFormula(const Address & address, const std::string & formula);

private:

    /// Disabled copy constructor
    Sheet(const Sheet &);

    /// Disabled copy assignment operator
    Sheet & operator=(const Sheet &);

    std::unique_ptr<Cells> m_pCells;

    int m_phase;
};
