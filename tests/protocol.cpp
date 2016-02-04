/*  Embedis - Embedded Dictionary Server
    Copyright (C) 2016 PatternAgents, LLC

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


TEST(Protocol, WithExtraSapces)
{
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


TEST(Protocol, Caps)
{
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


TEST(Protocol, Overflow)
{
    embedis_test_init();

    std::string s;

    // The 1 is for the trailing zero
    s.append(BUF_LENGTH-1, 'X');
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
    for (int i = 0; i < ARGV_LENGTH; i++) {
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


TEST(Protocol, Quotes)
{
    embedis_test_init();

    EXPECT_EMBEDIS_STRING("GET \"vendor\"", "PatternAgents");
    EXPECT_EMBEDIS_STRING("GET  \"vendor\"", "PatternAgents");
    EXPECT_EMBEDIS_STRING("GET \"vendor\" ", "PatternAgents");

    EXPECT_EMBEDIS_OK("SELECT RAM");

    EXPECT_EMBEDIS_OK("set foo \"\"");
    EXPECT_EMBEDIS_STRING("GET foo", "");

    EXPECT_EMBEDIS_OK("set    \"two words\"    \"you win\"   ");
    EXPECT_EMBEDIS_STRING("GET \"two words\"", "you win");

    // If this was written it would cause data loss
    EXPECT_EMBEDIS_ERROR("set \"\" doh ");

}


TEST(Protocol, Binary)
{
    embedis_test_init();

    std::vector<std::string> a;

    a = {"rom", "ram"};

    EXPECT_EMBEDIS_ARRAY("DICTIONARIES", a);
    EXPECT_EMBEDIS_OK("*2\r\n$6\r\nSELECT\r\n$3\r\nRAM\r\n");
    EXPECT_EMBEDIS_ARRAY("DICTIONARIES", a);
    EXPECT_EMBEDIS_ARRAY("*1\r\n$12\r\nDICTIONARIES\r\n", a);
    EXPECT_EMBEDIS_ARRAY("$12\r\nDICTIONARIES\r\n", a);
    EXPECT_EMBEDIS_ARRAY("*1\r\n+DICTIONARIES\r\n", a);
    EXPECT_EMBEDIS_ARRAY("+DICTIONARIES", a);
    EXPECT_EMBEDIS_ARRAY("*1\r\n-DICTIONARIES\r\n", a);
    EXPECT_EMBEDIS_ARRAY("-DICTIONARIES", a);
    EXPECT_EMBEDIS_ARRAY("*1\r\n:DICTIONARIES\r\n", a);
    EXPECT_EMBEDIS_ARRAY(":DICTIONARIES", a);
}
