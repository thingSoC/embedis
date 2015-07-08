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

int main() {
    testing::reporter(new testing::DefaultReporter);
    return testing::run();
}

// Helper for functional testing of Embedis commands.
// This mock uses strings instead of a serial port.

static std::string result;

void embedis_out(char data) {
    result.append(1, data);
}

std::string embedis(std::string cmd) {
    char last1 = 0, last2 = 0;
    result.clear();
    for (size_t i = 0; i < cmd.length(); i++) {
        embedis_in(cmd[i]);
        last1 = last2;
        last2 = cmd[i];
    }
    if (last1 != '\r' && last2 != '\n') {
        embedis_in('\r');
        embedis_in('\n');
    }
    return result;
}


// Testing predicates

namespace embedis_predicates {

static int delcrnl(std::string &s) {
    if (s.size() < 2) return 0;
    if (s.substr(s.size()-2, 2) != "\r\n") return 0;
    s.erase(s.size()-2,2);
    return 1;
}

std::string escaped(std::string const& s) {
    ::std::ostringstream out;
    out.put('"');
    for (std::string::const_iterator i = s.begin(), end = s.end(); i != end; ++i) {
        unsigned char c = *i;
        if (' ' <= c and c <= '~' and c != '\\' and c != '"') {
            out.put(c);
        }
        else {
            out.put('\\');
            switch(c) {
            case '"':
                out.put('"');
                break;
            case '\\':
                out.put('\\');
                break;
            case '\t':
                out.put('t');
                break;
            case '\r':
                out.put('r');
                break;
            case '\n':
                out.put('n');
                break;
            default:
                char const* const hexdig = "0123456789ABCDEF";
                out.put('x');
                out.put(hexdig[c >> 4]);
                out.put(hexdig[c & 0xF]);
            }
        }
    }
    out.put('"');
    return out.str();
}

testing::AssertionResult ok(const char* cmd_expr, std::string cmd) {
    std::string result = embedis(cmd);
    std::string s = result;
    if (!delcrnl(s)) goto fail;
    if (result[0] != '+') goto fail;
    return testing::AssertionSuccess();
fail:
    return testing::AssertionFailure() << cmd_expr <<
           " expected to succeed." << ::std::endl <<
           "Result was: " << escaped(result);
}

testing::AssertionResult fail(const char* cmd_expr, std::string cmd) {
    std::string result = embedis(cmd);
    std::string s = result;
    if (!delcrnl(s)) goto fail;
    if (result[0] != '-') goto fail;
    return testing::AssertionSuccess();
fail:
    return testing::AssertionFailure() << cmd_expr <<
           " expected to fail." << ::std::endl <<
           "Result was: " << escaped(result);
}

testing::AssertionResult null(const char* cmd_expr, std::string cmd) {
    std::string result = embedis(cmd);
    if (result == "$-1\r\n") {
        return testing::AssertionSuccess();
    }
    return testing::AssertionFailure() << cmd_expr <<
           " expected to return null." << ::std::endl <<
           "Result was: " << escaped(result);
}

testing::AssertionResult string(const char* cmd_expr, const char* result_expr, std::string cmd, std::string expected) {
    std::string result = embedis(cmd);
    std::string s = result;

    if (!s.empty()) {
        // Simple string
        if (s[0] == '+') {
            s.erase(0,1);
            if (!delcrnl(s)) goto fail;
            if (s == expected) return testing::AssertionSuccess();
        }
        // Binary string
        if (s[0] == '$') {
            s.erase(0,1);
            int size = 0;
            while (!s.empty() && s[0] >= '0' && s[0] <= '9') {
                size = size * 10 + (s[0] - '0');
                s.erase(0,1);
            }
            if (s.length() != size + 4) goto fail;
            if (s[0] != '\r') goto fail;
            if (s[1] != '\n') goto fail;
            s.erase(0,2);
            if (!delcrnl(s)) goto fail;
            if (s == expected) return testing::AssertionSuccess();
        }

    }

fail:
    return testing::AssertionFailure()  << cmd_expr <<
           " expected to return string: " << escaped(expected) << ::std::endl <<
           "Result was: " << escaped(result);
}

} /* end namespace embedis_predicates */


// This is a mock EEPROM which is simply stored in RAM.

#define FAKE_EEPROM_SIZE 128

static char fake_eeprom[FAKE_EEPROM_SIZE];

void fake_eeprom_erase() {
    for (size_t i = 0; i < FAKE_EEPROM_SIZE; i++) fake_eeprom[i] = 255;
}

size_t embedis_eeprom_size() {
    return FAKE_EEPROM_SIZE;
}

size_t embedis_eeprom_fetch(size_t pos) {
    if (pos >= FAKE_EEPROM_SIZE) throw;
    return fake_eeprom[pos];
}

void embedis_eeprom_store(size_t pos, char value) {
    if (pos >= FAKE_EEPROM_SIZE) throw;
    fake_eeprom[pos] = value;
}
