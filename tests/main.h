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

#include "benchtest.hpp"
#include "embedis.h"

#ifndef TESTMAIN_H
#define TESTMAIN_H

std::string embedis(std::string cmd);
void fake_ram_erase();

namespace embedis_predicates {

testing::AssertionResult ok(const char* cmd_expr, std::string cmd);
testing::AssertionResult error(const char* cmd_expr, std::string cmd);
testing::AssertionResult null(const char* cmd_expr, std::string cmd);
testing::AssertionResult string(const char* cmd_expr, const char* result_expr, std::string cmd, std::string result);

} /* end namespace embedis_predicates */

#define EXPECT_EMBEDIS_OK(val1) \
EXPECT_PRED_FORMAT1(embedis_predicates::ok, val1)
#define ASSERT_EMBEDIS_OK(val1) \
ASSERT_PRED_FORMAT1(embedis_predicates::ok, val1)

#define EXPECT_EMBEDIS_ERROR(val1) \
EXPECT_PRED_FORMAT1(embedis_predicates::error, val1)
#define ASSERT_EMBEDIS_FAIL(val1) \
ASSERT_PRED_FORMAT1(embedis_predicates::error, val1)

#define EXPECT_EMBEDIS_NULL(val1) \
EXPECT_PRED_FORMAT1(embedis_predicates::null, val1)
#define ASSERT_EMBEDIS_NULL(val1) \
ASSERT_PRED_FORMAT1(embedis_predicates::null, val1)

#define EXPECT_EMBEDIS_STRING(val1, val2) \
EXPECT_PRED_FORMAT2(embedis_predicates::string, val1, val2)
#define ASSERT_EMBEDIS_STRING(val1, val2) \
ASSERT_PRED_FORMAT2(embedis_predicates::string, val1, val2)


#endif // TESTMAIN_H
