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

testing::AssertionResult embedisOK(const char* cmd_expr, std::string cmd) {
    std::string result = embedis(cmd);
    if (result.empty() || result[0] != '+') {
        return testing::AssertionFailure() << cmd_expr <<
               " expected to succeed. Result was: " << result;
    }
    return testing::AssertionSuccess();
}

testing::AssertionResult embedisFAIL(const char* cmd_expr, std::string cmd) {
    std::string result = embedis(cmd);
    if (result.empty() || result[0] != '-') {
        return testing::AssertionFailure() << cmd_expr <<
               " expected to fail. Result was: " << result;
    }
    return testing::AssertionSuccess();
}



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
