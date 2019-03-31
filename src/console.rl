%%{

machine console;

main := |*

([A-Za-z]+[0-9]+)
    {
        address = getStr(ts, te);
    };

(['='"'"] any*)
    {
        formula = getStr(ts, te);
    };

(space)
    {
        // Ignore whitespace
    };

(any)
    {
        return false;
    };

*|;

}%%

#include <iostream>

#include "address.hpp"
#include "sheet.hpp"
#include "util.hpp"

%% write data;

bool eval(Sheet & sheet, const std::string & input)
{
    std::string address;
    std::string formula;

    int cs;
    const char * ts;
    const char * te;
    int act;

    %% write init;

    // Setup constants for lexical analyzer
    const char * p = input.c_str();
    const char * pe = input.c_str() + input.size();
    const char * eof = pe;

    %% write exec;

    if (address.size() > 0) {
        // An address has been defined
        const Address parsedAddress(address);
        const std::string currentFormula = sheet.getFormula(parsedAddress);
        if (formula.size() > 0) {
            // A formula has also been defined; update the appropriate cell...
            sheet.setFormula(parsedAddress, formula);
            try {
                // ...then attempt to re-calculate all cells in the spreadsheet
                sheet.recalculate();
            } catch (const std::runtime_error & e) {
                // If something goes wrong, restore the previous formula for the cell
                std::cout << "Error: " << e.what() << std::endl;
                if (currentFormula.size() > 0) {
                    sheet.setFormula(parsedAddress, currentFormula);
                } else {
                    sheet.erase(parsedAddress);
                }

            }
            sheet.recalculate();
            sheet.print();
        } else if (currentFormula.size() == 0) {
            // New formula has not been defined, and cell is empty, so we simply report it as being undefined
            std::cout << address << " is not defined." << std::endl;
        } else {
            // New formula has not been defined; so this is a query for the value/formula of an individual cell
            std::cout << address << " " << currentFormula << " = " << sheet.getValue(parsedAddress) << std::endl;
        }
    } else if (formula.size() > 0) {
        // Address has not been provided, but formula has; this is an error
        return false;
    }

    return true;
}

int main()
{
    Sheet sheet;

    while (std::cin) {
        std::cout << "> ";
        std::string input;
        std::getline(std::cin, input);
        if (!eval(sheet, input)) {
            std::cout << "Error: Invalid input." << std::endl;
        }
    }

    std::cout << std::endl;

    return 0;
}
