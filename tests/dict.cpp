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
#include "embedis.h"


TEST(DictROM, Select) {

    embedis_init();
    fake_eeprom_erase();

    EXPECT_PRED_FORMAT1(embedisOK, "SELECT ROM");

    EXPECT_EQ(
        embedis("GET vendor"),
        "+AE9RB\r\n"
    );

    EXPECT_PRED_FORMAT1(embedisOK, "SELECT EEPROM");

    EXPECT_PRED_FORMAT1(embedisOK, "DEL vendor");

    EXPECT_PRED_FORMAT1(embedisOK, "SELECT rom");

    EXPECT_EQ(
        embedis("GET vendor"),
        "+AE9RB\r\n"
    );

}



TEST(DictROM, Basics) {

    embedis_init();

    EXPECT_PRED_FORMAT1(embedisOK, "SELECT ROM");

    EXPECT_EQ(
        embedis("GET vendor"),
        "+AE9RB\r\n"
    );

    EXPECT_PRED_FORMAT1(embedisFAIL, "SET vendor blah");

    EXPECT_PRED_FORMAT1(embedisFAIL, "DEL vendor");

}

TEST(DictEEPROM, Basics) {
    std::string s;

    embedis_init();
    fake_eeprom_erase();

    EXPECT_PRED_FORMAT1(embedisOK, "SELECT EEPROM");

    EXPECT_PRED_FORMAT1(embedisOK, "SET foo1 bar1");

    EXPECT_PRED_FORMAT1(embedisOK, "SET foo bar");

    s = "SET ";
    s += embedis_dictionary_keys[0].name;
    s += " bar2";
    EXPECT_PRED_FORMAT1(embedisOK, s);


    EXPECT_PRED_FORMAT1(embedisOK, "SET foo good");

    EXPECT_PRED_FORMAT1(embedisOK, "SET foo3 bar3");

    EXPECT_EQ(
        embedis("GET foo"),
        "$4\r\ngood\r\n"
    );

    EXPECT_PRED_FORMAT1(embedisOK, "DEL foo");

    EXPECT_EQ(
        embedis("GET foo"),
        "$-1\r\n"
    );

    EXPECT_EQ(
        embedis("GET foo1"),
        "$4\r\nbar1\r\n"
    );

    s = "GET ";
    s += embedis_dictionary_keys[0].name;
    EXPECT_EQ(
        embedis(s.c_str()),
        "$4\r\nbar2\r\n"
    );

    EXPECT_EQ(
        embedis("GET foo3"),
        "$4\r\nbar3\r\n"
    );

    s = "DEL ";
    s += embedis_dictionary_keys[0].name;
    EXPECT_PRED_FORMAT1(embedisOK, s);

    s = "GET ";
    s += embedis_dictionary_keys[0].name;
    EXPECT_EQ(
        embedis(s.c_str()),
        "$-1\r\n"
    );


}

TEST(DictEEPROM, KEYS) {
    std::string s;

    embedis_init();
    fake_eeprom_erase();

    EXPECT_PRED_FORMAT1(embedisOK, "SELECT EEPROM");

    EXPECT_EQ(
        embedis("KEYS"),
        "*0\r\n"
    );

    EXPECT_PRED_FORMAT1(embedisOK, "SET foo bar");

    EXPECT_EQ(
        embedis("KEYS"),
        "*1\r\n$3\r\nfoo\r\n"
    );

    s = "SET ";
    s += embedis_dictionary_keys[0].name;
    s += " bar2";
    EXPECT_PRED_FORMAT1(embedisOK, s);

    s = "*2\r\n$3\r\nfoo\r\n+";
    s += embedis_dictionary_keys[0].name;
    s += "\r\n";
    EXPECT_EQ(
        embedis("KEYS"),
        s
    );


}
