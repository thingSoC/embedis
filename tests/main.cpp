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

#include "benchtest.hpp"
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

std::string embedis(const char* cmd) {
    char last1 = 0, last2 = 0;
    result.clear();
    while (*cmd) {
        embedis_in(*cmd);
        last1 = last2;
        last2 = *cmd;
        cmd++;
    }
    if (last1 != '\r' && last2 != '\n') {
        embedis_in('\r');
        embedis_in('\n');
    }
    return result;
}



// This is a mock NVRAM which is simply stored in RAM.

#define FAKE_NVRAM_SIZE 128

static char fake_nvram[FAKE_NVRAM_SIZE];

size_t embedis_nvram_size() {
    return FAKE_NVRAM_SIZE;
}

size_t embedis_nvram_fetch(size_t pos) {
    if (pos >= FAKE_NVRAM_SIZE) throw;
    return fake_nvram[pos];
}

void embedis_nvram_store(size_t pos, char value) {
    if (pos >= FAKE_NVRAM_SIZE) throw;
    fake_nvram[pos] = value;
}
