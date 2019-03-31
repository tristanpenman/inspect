/*
 * test/SheetTest.cpp
 *
 * Copyright (c) 2012 Tristan Penman
 *
 * ----------------------------------------------------------------------------
 *
 * This file is part of Inspect.
 *
 * Inspect is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <map>
#include <iostream>

#include "gtest/gtest.h"

#include "address.hpp"
#include "formula.hpp"
#include "sheet.hpp"

using namespace std;

class SheetTest : public testing::Test
{

};

TEST_F(SheetTest, setFormula_and_getFormula_basic)
{
    Sheet sheet;
    Address address("A1");
    EXPECT_EQ(1, address.column);
    EXPECT_EQ(1, address.row);

    typedef map<string, string> Formulas;

    Formulas formulas;
    formulas["=1+1"] = "2";
    formulas["=1.5 + 1"] = "2.5";
    formulas["=1 + 1.5"] = "2.5";
    formulas["=\"Hello\"+\" \"+\"World\""] = "Hello World";
    formulas["=\"Hello\"+3.25"] = "Hello3.25";
    formulas["=3.25+\"Hello\""] = "3.25Hello";

    for (Formulas::const_iterator itr = formulas.begin(); itr != formulas.end(); itr++) {
        const string & expectedFormula = itr->first;
        EXPECT_TRUE(sheet.setFormula(address, expectedFormula));

        string retrievedFormula = sheet.getFormula(address);
        EXPECT_EQ(expectedFormula, retrievedFormula);

        sheet.recalculate();

        const string & expectedValue = itr->second;
        string retrievedValue = sheet.getValue(address);
        EXPECT_EQ(expectedValue, retrievedValue);
    }
}

TEST_F(SheetTest, setFormula_and_getFormula_with_addresses)
{
    Sheet sheet;

    Address address1("A1");
    EXPECT_EQ(1, address1.column);
    EXPECT_EQ(1, address1.row);

    Address address2("B2");
    EXPECT_EQ(2, address2.column);
    EXPECT_EQ(2, address2.row);

    // Set formula for A1 with no references
    const string formula1 = "=1+12.5";
    EXPECT_TRUE(sheet.setFormula(address1, formula1));

    // Set formula for B2 so that it references A1
    const string formula2 = "=A1+12";
    EXPECT_TRUE(sheet.setFormula(address2, formula2));

    sheet.recalculate();

    const string & expectedValue = "25.5";
    string retrievedValue = sheet.getValue(address2);
    EXPECT_EQ(expectedValue, retrievedValue);
}
