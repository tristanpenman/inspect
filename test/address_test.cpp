/*
 * test/AddressTest.cpp
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

#include <stdexcept>

#include "address.hpp"

#include "gtest/gtest.h"

class AddressTest : public testing::Test
{

};

TEST_F(AddressTest, constructionFromColumnAndRow)
{
    EXPECT_NO_THROW({
        Address address(1, 1);
        EXPECT_EQ(1, address.column);
        EXPECT_EQ(1, address.row);
    });

    EXPECT_NO_THROW({
        Address address(2, 2);
        EXPECT_EQ(2, address.column);
        EXPECT_EQ(2, address.row);
    });

    EXPECT_NO_THROW({
        Address address(27, 11);
        EXPECT_EQ(27, address.column);
        EXPECT_EQ(11, address.row);
    });

    EXPECT_NO_THROW({
        Address address(0, 0);
        EXPECT_EQ(0, address.column);
        EXPECT_EQ(0, address.row);
    });
}

TEST_F(AddressTest, constructionFromString)
{
    EXPECT_NO_THROW({
        Address address("A1");
        EXPECT_EQ(1, address.column);
        EXPECT_EQ(1, address.row);
    });

    EXPECT_NO_THROW({
        Address address("B2");
        EXPECT_EQ(2, address.column);
        EXPECT_EQ(2, address.row);
    });

    EXPECT_NO_THROW({
        Address address("AA11");
        EXPECT_EQ(27, address.column);
        EXPECT_EQ(11, address.row);
    });

    EXPECT_THROW(Address("11AA"), std::invalid_argument);
    EXPECT_THROW(Address("A_1"), std::invalid_argument);
    EXPECT_THROW(Address("11"), std::invalid_argument);
    EXPECT_THROW(Address(""), std::invalid_argument);
}
