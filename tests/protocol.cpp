/*  Embedis - Embedded Dictionary Server
    Copyright (C) 2015 PatternAgents, LLC

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

#include "main.h"

TEST(Protocol, WithExtraSapces) {

    embedis_test_init();

    EXPECT_EQ(
        embedis_test("GET vendor"),
        "+PatternAgents\r\n"
    );

    EXPECT_EQ(
        embedis_test("  GET vendor"),
        "+PatternAgents\r\n"
    );

    EXPECT_EQ(
        embedis_test("GET   vendor"),
        "+PatternAgents\r\n"
    );

    EXPECT_EQ(
        embedis_test("GET vendor  "),
        "+PatternAgents\r\n"
    );

}


TEST(Protocol, Caps) {

    embedis_test_init();

    EXPECT_EQ(
        embedis_test("get vendor"),
        "+PatternAgents\r\n"
    );

    EXPECT_EQ(
        embedis_test("GET vendor"),
        "+PatternAgents\r\n"
    );

    EXPECT_EQ(
        embedis_test("GeT vendor"),
        "+PatternAgents\r\n"
    );

}

TEST(Protocol, Overflow) {

    embedis_test_init();

    std::string s;

    // Find instance 0 which is used for main testing
    embedis_state* state = embedis_state_last(0);
    while (state->num) state = state->prev;

    // The 1 is for the trailing zero
    s.append(state->protocol.buf_length-1, 'X');
    EXPECT_EQ(
        embedis_test(s.c_str()),
        "-ERROR unknown command\r\n"
    );

    // This one should overflow
    s.append(1, 'X');
    EXPECT_EQ(
        embedis_test(s.c_str()),
        "-ERROR buffer overflow\r\n"
    );

    // Make sure bad things don't happen at max args
    s.clear();
    for (int i = 0; i < state->protocol.argv_length; i++) {
        s.append("Z ");
    }
    EXPECT_EQ(
        embedis_test(s.c_str()),
        "-ERROR unknown command\r\n"
    );

    // Make sure we report args overflow
    s.append("Z ");
    EXPECT_EQ(
        embedis_test(s.c_str()),
        "-ERROR bad argument count\r\n"
    );

    // Make sure bad things don't happen with more than max args
    s.append("Z Z Z Z");
    EXPECT_EQ(
        embedis_test(s.c_str()),
        "-ERROR bad argument count\r\n"
    );


}
