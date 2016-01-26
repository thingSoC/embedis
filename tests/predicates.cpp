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


namespace embedis_predicates {

// Remove expected "\r\n"
// return true on success
static bool delstartcrnl(std::string &s)
{
    if (s.size() < 2) return false;
    if (s.substr(0, 2) != "\r\n") return false;
    s.erase(0,2);
    return true;
}

// Extract first string from the RESP result
// return true on success
static bool getstring(std::string &s, std::string &out)
{
    out.clear();

    if (s.empty()) return false;

    // Simple string, including error string
    // Integers are treated as strings too
    if (s[0] == ':' || s[0] == '+' || s[0] == '-') {
        s.erase(0,1);
        while (!s.empty() && s[0] != '\r') {
            out.append(1, s[0]);
            s.erase(0,1);
        }
        return delstartcrnl(s);
    }

    // Binary string
    if (s[0] == '$') {
        s.erase(0,1);
        int size = 0;
        while (!s.empty() && s[0] >= '0' && s[0] <= '9') {
            size = size * 10 + (s[0] - '0');
            s.erase(0,1);
        }
        if (!delstartcrnl(s)) return false;
        while (!s.empty() && size) {
            size--;
            out.append(1, s[0]);
            s.erase(0,1);
        }
        return delstartcrnl(s);
    }
    return false;
}

// Utility to quote and add escape sequences for printing string
std::string escaped(std::string const& s)
{
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

testing::AssertionResult ok(const char* cmd_expr, std::string cmd)
{
    std::string result = embedis_test(cmd);
    std::string s = result;
    std::string actual;
    if (!s.empty() && result[0] == '+') {
        if (getstring(s, actual)) {
            if (s.empty() && actual.substr(0,2) == "OK") {
                return testing::AssertionSuccess();
            }
        }

    }
    return testing::AssertionFailure() << cmd_expr <<
           " expected to succeed." << ::std::endl <<
           "Result was: " << escaped(result);
}

testing::AssertionResult error(const char* cmd_expr, std::string cmd)
{
    std::string result = embedis_test(cmd);
    std::string s = result;
    std::string actual;
    if (!s.empty() && result[0] == '-') {
        if (getstring(s, actual)) {
            if (s.empty()) {
                return testing::AssertionSuccess();
            }
        }

    }
    return testing::AssertionFailure() << cmd_expr <<
           " expected to fail." << ::std::endl <<
           "Result was: " << escaped(result);
}

testing::AssertionResult null(const char* cmd_expr, std::string cmd)
{
    std::string result = embedis_test(cmd);
    if (result == "$-1\r\n") {
        return testing::AssertionSuccess();
    }
    return testing::AssertionFailure() << cmd_expr <<
           " expected to return null." << ::std::endl <<
           "Result was: " << escaped(result);
}

testing::AssertionResult string(const char* cmd_expr, const char* result_expr, std::string cmd, std::string expected)
{
    std::string result = embedis_test(cmd);
    std::string s = result;
    std::string actual;
    if (getstring(s, actual)) {
        if (actual==expected && s.empty()) {
            return testing::AssertionSuccess();
        }

    }
    return testing::AssertionFailure()  << cmd_expr <<
           " expected to return string: " << escaped(expected) << ::std::endl <<
           "Result was: " << escaped(result);
}


testing::AssertionResult array(const char* cmd_expr, const char* result_expr, std::string cmd, std::vector<std::string> expected)
{
    std::string result = embedis_test(cmd);
    std::string s = result;
    std::string actual;

    if (s.empty()) goto fail;
    while (!s.empty() && s[0] == '*') {
        int size = 0;
        s.erase(0,1);
        while (!s.empty() && s[0] >= '0' && s[0] <= '9') {
            size = size * 10 + (s[0] - '0');
            s.erase(0,1);
        }
        if (!delstartcrnl(s)) goto fail;
        if (size != expected.size()) goto fail;

        for (auto e : expected ) {
            if (!getstring(s, actual)) goto fail;
            if (e != actual) goto fail;
        }
    }
    if (!s.empty()) goto fail;
    return testing::AssertionSuccess();

fail:
    std::string eprint;
    eprint.append("[");
    for (auto e : expected ) {
        eprint.append(escaped(e));
        eprint.append(", ");
    }
    if (eprint.size() > 2) eprint.erase(eprint.size()-2,2);
    eprint.append("]");
    return testing::AssertionFailure()  << cmd_expr <<
           " expected to return array: " << eprint << ::std::endl <<
           "Result was: " << escaped(result);
}

} /* end namespace embedis_predicates */
