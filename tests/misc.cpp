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


TEST(Commands, Basic)
{
    embedis_test_init();

    // Make sure internal commands is really set up
    // before we try to peek at them from here.
    embedis_test("commands");

    // Access to protected member
    class EmbedisBypass : public Embedis {
    public:
        static TVec<Command>* c() {
            return &commands;
        }
    };
    auto commands = EmbedisBypass::c();

    std::vector<std::string> a;
    for (size_t i = 0; i < commands->size(); ++i) {
        a.push_back((*commands)[i].name.c_str());
    }

    EXPECT_EMBEDIS_ARRAY("COMMANDS", a);
}
