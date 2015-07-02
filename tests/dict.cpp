/*  Embedis - Embedded Dictionary Server
    Copyright (C) 2015 Pattern Agents, LLC

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <benchtest.hpp>
#include "main.h"
#include "embedis.h"


TEST(DictROM, Basics){

    embedis_reset();

    EXPECT_EQ(
              embedis("GET vendor"),
              "+AE9RB\r\n"
              ) << "Retrieve ROM value";;

    EXPECT_EQ(
              embedis("SET vendor blah"),
              "-ERROR\r\n"
              ) << "ROM values are read-only";

    EXPECT_EQ(
              embedis("DEL vendor"),
              "-ERROR\r\n"
              ) << "ROM values are read-only";


}

TEST(DictNVRAM, Basics){

    embedis_reset();

    EXPECT_EQ(
              embedis("SET foo1 bar1"),
              "+OK\r\n"
              );

    EXPECT_EQ(
              embedis("SET foo bar"),
              "+OK\r\n"
              );

    EXPECT_EQ(
              embedis("SET foo2 bar2"),
              "+OK\r\n"
              );

    EXPECT_EQ(
              embedis("SET foo good"),
              "+OK\r\n"
              );

    EXPECT_EQ(
              embedis("SET foo3 bar3"),
              "+OK\r\n"
              );

    EXPECT_EQ(
              embedis("GET foo"),
              "$4\r\ngood\r\n"
              );

    EXPECT_EQ(
              embedis("DEL foo"),
              "+OK\r\n"
              );

    EXPECT_EQ(
              embedis("GET foo"),
              "$-1\r\n"
              );

    EXPECT_EQ(
              embedis("GET foo1"),
              "$4\r\nbar1\r\n"
              );

    EXPECT_EQ(
              embedis("GET foo2"),
              "$4\r\nbar2\r\n"
              );

    EXPECT_EQ(
              embedis("GET foo3"),
              "$4\r\nbar3\r\n"
              );


}
