/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <gtest/gtest.h>

#include "global/types/number_t.h"

using namespace mu;

class Global_Types_NumberTests : public ::testing::Test
{
public:
};

TEST_F(Global_Types_NumberTests, Integer)
{
    {
        integer_t val;
        EXPECT_EQ(val, 0);
    }

    {
        integer_t val = 42;
        EXPECT_EQ(val, 42);
    }

    {
        integer_t val;
        val = 42;
        EXPECT_EQ(val, 42);
        val = 22;
        EXPECT_EQ(val, 22);
    }

    {
        integer_t val = 42;
        EXPECT_TRUE(val < 44);
        EXPECT_TRUE(val > 40);
        EXPECT_TRUE(val <= 42);
        EXPECT_TRUE(val >= 42);
    }

    {
        integer_t val1 = 42;
        integer_t val2 = 22;
        integer_t val3 = val1 + val2;
        EXPECT_EQ(val3, 64);
        integer_t val4 = val3 - val2;
        EXPECT_EQ(val4, 42);
    }

    {
        integer_t val1 = 42;
        integer_t val2 = val1 + 22;
        EXPECT_EQ(val2, 64);
        integer_t val3 = val2 - 60;
        EXPECT_EQ(val3, 4);
    }

    {
        integer_t val1 = 42;
        integer_t val2 = 22;
        val2 += val1;
        EXPECT_EQ(val2, 64);
        val2 -= val1;
        EXPECT_EQ(val2, 22);
        val2 += 16;
        EXPECT_EQ(val2, 38);
        val2 -= 18;
        EXPECT_EQ(val2, 20);
    }

    {
        integer_t val1 = 42;
        integer_t val2 = 21;
        integer_t val3 = val1 / val2;
        EXPECT_EQ(val3, 2);
        val3 = val1 * val2;
        EXPECT_EQ(val3, 882);
        val3 /= 2;
        EXPECT_EQ(val3, 441);
        val3 *= 3;
        EXPECT_EQ(val3, 1323);
    }
}
