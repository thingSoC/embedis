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

TEST(Commands, Basic) {
    embedis_test_init();

    std::vector<std::string> a;
    const embedis_command *cmd = &embedis_commands[0];
    while (cmd->name) {
        a.push_back(cmd->name);
        cmd++;
    }
    EXPECT_EMBEDIS_ARRAY("COMMANDS", a);
}
