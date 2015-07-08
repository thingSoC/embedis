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

#include "main.h"


TEST(DictROM, Select) {

    embedis_init();
    fake_eeprom_erase();

    ASSERT_EMBEDIS_FAIL("SELECT this_does_not_exist");
    ASSERT_EMBEDIS_OK("SELECT ROM");
    EXPECT_EMBEDIS_STRING("GET vendor", "AE9RB");
    ASSERT_EMBEDIS_OK("SELECT EEPROM");
    EXPECT_EMBEDIS_OK("DEL vendor");
    ASSERT_EMBEDIS_OK("SELECT rom");
    EXPECT_EMBEDIS_STRING("GET vendor", "AE9RB");
}


TEST(DictROM, Basics) {

    embedis_init();

    EXPECT_EMBEDIS_OK("SELECT ROM");
    EXPECT_EMBEDIS_STRING("GET vendor", "AE9RB");
    EXPECT_EMBEDIS_FAIL("SET vendor blah");
    EXPECT_EMBEDIS_FAIL("DEL vendor");
}


TEST(DictEEPROM, Basics) {
    std::string s;

    embedis_init();
    fake_eeprom_erase();

    EXPECT_EMBEDIS_OK("SELECT EEPROM");
    EXPECT_EMBEDIS_OK("SET foo1 bar1");
    EXPECT_EMBEDIS_OK("SET foo bar");

    s = "SET ";
    s += embedis_dictionary_keys[0].name;
    s += " bar2";
    EXPECT_EMBEDIS_OK(s);

    EXPECT_EMBEDIS_OK("SET foo good");
    EXPECT_EMBEDIS_OK("SET foo3 bar3");
    EXPECT_EMBEDIS_STRING("GET foo", "good");
    EXPECT_EMBEDIS_OK("DEL foo");
    EXPECT_EMBEDIS_NULL("GET foo");
    EXPECT_EMBEDIS_STRING("GET foo1", "bar1");

    s = "GET ";
    s += embedis_dictionary_keys[0].name;
    EXPECT_EMBEDIS_STRING(s, "bar2");

    EXPECT_EMBEDIS_STRING("GET foo3", "bar3");

    s = "DEL ";
    s += embedis_dictionary_keys[0].name;
    EXPECT_EMBEDIS_OK(s);

    s = "GET ";
    s += embedis_dictionary_keys[0].name;
    EXPECT_EMBEDIS_NULL(s);
}

TEST(DictEEPROM, KEYS) {
    std::string s;

    embedis_init();
    fake_eeprom_erase();

    EXPECT_EMBEDIS_OK("SELECT EEPROM");

    EXPECT_EQ(
        embedis("KEYS"),
        "*0\r\n"
    );

    EXPECT_EMBEDIS_OK("SET foo bar");

    EXPECT_EQ(
        embedis("KEYS"),
        "*1\r\n$3\r\nfoo\r\n"
    );

    s = "SET ";
    s += embedis_dictionary_keys[0].name;
    s += " bar2";
    EXPECT_EMBEDIS_OK(s);

    s = "*2\r\n$3\r\nfoo\r\n+";
    s += embedis_dictionary_keys[0].name;
    s += "\r\n";
    EXPECT_EQ(
        embedis("KEYS"),
        s
    );

}
