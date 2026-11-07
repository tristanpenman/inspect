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
#include <string>

#include "gtest/gtest.h"

#include "address.h"
#include "formula.h"
#include "sheet.h"

class SheetTest : public testing::Test
{
};

TEST_F(SheetTest, setFormulaAndGetFormulaBasic)
{
    Sheet sheet;
    Address address("A1");
    EXPECT_EQ(1, address.column);
    EXPECT_EQ(1, address.row);

    const std::map<std::string, std::string> formulas = {
        {"=1+1", "2"},
        {"=1.5 + 1", "2.5"},
        {"=1 + 1.5", "2.5"},
        {"=\"Hello\"+\" \"+\"World\"", "Hello World"},
        {"=\"Hello\"+3.25", "Hello3.25"},
        {"=3.25+\"Hello\"", "3.25Hello"}
    };

    for (const auto& formula : formulas) {
        const std::string& expectedFormula = formula.first;
        EXPECT_TRUE(sheet.setFormula(address, expectedFormula));

        const std::string retrievedFormula = sheet.getFormula(address);
        EXPECT_EQ(expectedFormula, retrievedFormula);

        sheet.recalculate();

        const std::string& expectedValue = formula.second;
        const std::string retrievedValue = sheet.getValue(address);
        EXPECT_EQ(expectedValue, retrievedValue);
    }
}

TEST_F(SheetTest, setFormulaAndGetFormulaWithAddresses)
{
    Sheet sheet;

    Address address1("A1");
    EXPECT_EQ(1, address1.column);
    EXPECT_EQ(1, address1.row);

    Address address2("B2");
    EXPECT_EQ(2, address2.column);
    EXPECT_EQ(2, address2.row);

    // Set formula for A1 with no references
    const std::string formula1 = "=1+12.5";
    EXPECT_TRUE(sheet.setFormula(address1, formula1));

    // Set formula for B2 so that it references A1
    const std::string formula2 = "=A1+12";
    EXPECT_TRUE(sheet.setFormula(address2, formula2));

    sheet.recalculate();

    const std::string expectedValue = "25.5";
    const std::string retrievedValue = sheet.getValue(address2);
    EXPECT_EQ(expectedValue, retrievedValue);
}

TEST_F(SheetTest, formulaOwnsFunctionCallArguments)
{
    Formula formula("=SUM(1,2)");

    EXPECT_EQ("fn{SUM}(1,2)", std::string(formula));
}
