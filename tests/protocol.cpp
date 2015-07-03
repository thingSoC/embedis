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


TEST(Protocol, WithExtraSapces) {

    embedis_init();

    EXPECT_EQ(
        embedis("GET vendor"),
        "+AE9RB\r\n"
    );

    EXPECT_EQ(
        embedis("  GET vendor"),
        "+AE9RB\r\n"
    );

    EXPECT_EQ(
        embedis("GET   vendor"),
        "+AE9RB\r\n"
    );

    EXPECT_EQ(
        embedis("GET vendor  "),
        "+AE9RB\r\n"
    );

}


TEST(Protocol, Caps) {

    embedis_init();

    EXPECT_EQ(
        embedis("get vendor"),
        "+AE9RB\r\n"
    );

    EXPECT_EQ(
        embedis("GET vendor"),
        "+AE9RB\r\n"
    );

    EXPECT_EQ(
        embedis("GeT vendor"),
        "+AE9RB\r\n"
    );

}

TEST(Protocol, Overflow) {

    embedis_init();

    std::string s;

    // The 1 is for the trailing zero
    s.append(EMBEDIS_COMMAND_BUF_SIZE-1, 'X');
    EXPECT_EQ(
        embedis(s.c_str()),
        "-ERROR unknown command\r\n"
    );

    // This one should overflow
    s.append(1, 'X');
    EXPECT_EQ(
        embedis(s.c_str()),
        "-ERROR buffer overflow\r\n"
    );

    s.clear();
    for (int i = 0; i < EMBEDIS_COMMAND_MAX_ARGS; i++) {
        s.append("Z ");
    }
    EXPECT_EQ(
        embedis(s.c_str()),
        "-ERROR unknown command\r\n"
    );

    s.append("Z ");
    EXPECT_EQ(
        embedis(s.c_str()),
        "-ERROR bad argument count\r\n"
    );

    s.append("Z Z Z Z");
    EXPECT_EQ(
        embedis(s.c_str()),
        "-ERROR bad argument count\r\n"
    );


}
