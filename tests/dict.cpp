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


TEST(DictROM, Select)
{
    embedis_test_init();

    ASSERT_EMBEDIS_ERROR("SELECT this_does_not_exist");
    ASSERT_EMBEDIS_OK("SELECT ROM");
    EXPECT_EMBEDIS_STRING("GET vendor", "PatternAgents");
    ASSERT_EMBEDIS_OK("SELECT RAM");
    EXPECT_EMBEDIS_STRING("DEL vendor", "0");
    ASSERT_EMBEDIS_OK("SELECT rom");
    EXPECT_EMBEDIS_STRING("GET vendor", "PatternAgents");
}


TEST(DictROM, Basics)
{
    embedis_test_init();

    EXPECT_EMBEDIS_OK("SELECT ROM");
    EXPECT_EMBEDIS_STRING("GET vendor", "PatternAgents");
    EXPECT_EMBEDIS_ERROR("SET vendor blah");
    EXPECT_EMBEDIS_STRING("DEL vendor", "0");
}


TEST(DictROM, Keys)
{
    embedis_test_init();

    EXPECT_EMBEDIS_OK("SELECT ROM");
    EXPECT_EMBEDIS_ARRAY("KEYS", {"vendor"});
}


TEST(DictRAM, Basics)
{
    embedis_test_init();

    Embedis::key(F("7key7"), 7);

    EXPECT_EMBEDIS_OK("SELECT RAM");
    EXPECT_EMBEDIS_OK("SET foo1 bar1");
    EXPECT_EMBEDIS_OK("SET foo bar");

    EXPECT_EMBEDIS_OK("SET 7key7 bar2");

    EXPECT_EMBEDIS_OK("SET foo good");
    EXPECT_EMBEDIS_OK("SET foo3 bar3");
    EXPECT_EMBEDIS_STRING("GET foo", "good");
    EXPECT_EMBEDIS_STRING("DEL foo", "1");
    EXPECT_EMBEDIS_NULL("GET foo");
    EXPECT_EMBEDIS_STRING("GET foo1", "bar1");

    EXPECT_EMBEDIS_STRING("GET 7key7", "bar2");

    EXPECT_EMBEDIS_STRING("GET foo3", "bar3");

    EXPECT_EMBEDIS_STRING("DEL 7key7", "1");

    EXPECT_EMBEDIS_NULL("GET 7key7");
}


TEST(DictRAM, KEYS)
{
    embedis_test_init();

    Embedis::key(F("3key3"), 3);

    EXPECT_EMBEDIS_OK("SELECT RAM");
    EXPECT_EMBEDIS_ARRAY("KEYS", {});

    EXPECT_EMBEDIS_OK("SET foo bar");
    EXPECT_EMBEDIS_ARRAY("KEYS", {"foo"});

    EXPECT_EMBEDIS_OK("set 3key3 bar2");

    std::vector<std::string> a;
    a.push_back("foo");
    a.push_back("3key3");
    EXPECT_EMBEDIS_ARRAY("KEYS", a);
}


TEST(DictRAM, ValueOverflow)
{
    std::string s1, s2;
    size_t len;

    embedis_test_init();

    EXPECT_EMBEDIS_OK("SELECT RAM");

    // free space for a key of len 3
    len = kvs_data.size();
    len -= 2 + 2 + 3 + 2;

    // Just barely fits
    s2.assign(len, 'x');
    s1 = "SET foo ";
    s1 += s2;
    EXPECT_EMBEDIS_OK(s1);
    EXPECT_EMBEDIS_STRING("GET foo", s2);

    // One extra value char causes fail
    s2.assign(len+1, 'x');
    s1 = "SET foo ";
    s1 += s2;
    EXPECT_EMBEDIS_OK("SET foo bar");
    EXPECT_EMBEDIS_ERROR(s1);
    EXPECT_EMBEDIS_STRING("GET foo", "bar");
    EXPECT_EMBEDIS_STRING("DEL foo", "1");
}


TEST(DictRAM, KeyOverflow)
{
    std::string s1, s2, s3;
    size_t len;

    embedis_test_init();

    EXPECT_EMBEDIS_OK("SELECT RAM");

    // free key space for a value of len 3
    len = kvs_data.size();
    len -= 2 + 2 + 3 + 2;

    // Just barely fits
    s2.assign(len, 'x');
    s1 = "SET ";
    s1 += s2 + " bar";
    EXPECT_EMBEDIS_OK(s1);
    s1 = "GET ";
    s1 += s2;
    EXPECT_EMBEDIS_STRING(s1, "bar");

    // One extra value char causes fail
    s3.assign(len+1, 'x');
    s1 = "SET foo ";
    s1 += s3 + " bar";
    EXPECT_EMBEDIS_ERROR(s1);
    EXPECT_EMBEDIS_STRING("DEL foo", "0");

    s1 = "DEL ";
    s1 += s2;
    EXPECT_EMBEDIS_STRING(s1, "1");
}


TEST(DictRAM, SetAPI)
{
    embedis_test_init();

    bool rv;

    rv = Embedis::set("ram", "foo", "bar");
    EXPECT_TRUE(rv);
    EXPECT_EMBEDIS_OK("SELECT RAM");
    EXPECT_EMBEDIS_STRING("GET foo", "bar");

    rv = Embedis::set("ram", "foo", "doh");
    EXPECT_TRUE(rv);
    EXPECT_EMBEDIS_STRING("GET foo", "doh");

    rv = Embedis::set("vendor", "nocando");
    EXPECT_FALSE(rv);
}


TEST(DictRAM, GetAPI)
{
    embedis_test_init();

    String value;
    bool rv;

    rv = Embedis::get("ram", "foo", value);
    EXPECT_FALSE(rv);
    EXPECT_EMBEDIS_OK("SELECT RAM");
    EXPECT_EMBEDIS_NULL("get foo");
    EXPECT_EMBEDIS_OK("set foo baz");
    rv = Embedis::get("ram", "foo", value);
    EXPECT_TRUE(rv);
    EXPECT_EQ(value, "baz");

    rv = Embedis::get("vendor", value);
    EXPECT_TRUE(rv);
    EXPECT_EQ(value, "PatternAgents");
}


TEST(DictRAM, DelAPI)
{
    embedis_test_init();

    bool rv;

    rv = Embedis::del("ram", "foo");
    EXPECT_FALSE(rv);
    EXPECT_EMBEDIS_OK("SELECT RAM");
    EXPECT_EMBEDIS_NULL("get foo");
    EXPECT_EMBEDIS_OK("set foo baz");
    EXPECT_EMBEDIS_STRING("GET foo", "baz");
    rv = Embedis::del("ram", "foo");
    EXPECT_TRUE(rv);
    EXPECT_EMBEDIS_NULL("get foo");

    rv = Embedis::del("vendor");
    EXPECT_FALSE(rv);
}


TEST(Dict, DICTIONARIES) {
    embedis_test_init();

    std::vector<std::string> a;

    a = {"rom", "ram"};
    EXPECT_EMBEDIS_ARRAY("DICTIONARIES", a);
}
