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


TEST(IO, ReadWrite)
{
    embedis_test_init();

    Embedis::hardware("mock0", [](Embedis* e) {
        e->response("ZERO");
    }, 0);

    Embedis::hardware("mock1", 0, [](Embedis* e) {
        e->response(Embedis::OK);
    });


    EXPECT_EMBEDIS_ERROR("WRITE mock0");
    EXPECT_EMBEDIS_OK("WRITE mock1");
    EXPECT_EMBEDIS_ERROR("WRITE mock2");

    EXPECT_EMBEDIS_STRING("READ mock0", "ZERO");
    EXPECT_EMBEDIS_ERROR("READ mock1");
    EXPECT_EMBEDIS_ERROR("READ mock2");

    std::vector<std::string> a;

    a = {"RO: mock0", "WO: mock1"};
    EXPECT_EMBEDIS_ARRAY("HARDWARE", a);
}
