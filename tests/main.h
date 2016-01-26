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

#ifndef TESTMAIN_H
#define TESTMAIN_H


#include "benchtest.hpp"
#include "Embedis.h"
#include "Stream.h"

const size_t BUF_LENGTH = 128;
const size_t ARGV_LENGTH = 8;

std::string embedis_test(std::string cmd);
void embedis_test_interface(int i);
void embedis_test_init();

extern std::vector<char> kvs_data;

namespace embedis_predicates {

testing::AssertionResult ok(const char* cmd_expr, std::string cmd);
testing::AssertionResult error(const char* cmd_expr, std::string cmd);
testing::AssertionResult null(const char* cmd_expr, std::string cmd);
testing::AssertionResult string(const char* cmd_expr, const char* result_expr, std::string cmd, std::string result);
testing::AssertionResult array(const char* cmd_expr, const char* result_expr, std::string cmd, std::vector<std::string> expected);

} /* end namespace embedis_predicates */

inline void PrintTo(const String& val, ::std::ostream* os) {
    *os << val.c_str();
}

#define EXPECT_EMBEDIS_OK(val1) \
EXPECT_PRED_FORMAT1(embedis_predicates::ok, val1)
#define ASSERT_EMBEDIS_OK(val1) \
ASSERT_PRED_FORMAT1(embedis_predicates::ok, val1)

#define EXPECT_EMBEDIS_ERROR(val1) \
EXPECT_PRED_FORMAT1(embedis_predicates::error, val1)
#define ASSERT_EMBEDIS_ERROR(val1) \
ASSERT_PRED_FORMAT1(embedis_predicates::error, val1)

#define EXPECT_EMBEDIS_NULL(val1) \
EXPECT_PRED_FORMAT1(embedis_predicates::null, val1)
#define ASSERT_EMBEDIS_NULL(val1) \
ASSERT_PRED_FORMAT1(embedis_predicates::null, val1)

#define EXPECT_EMBEDIS_STRING(val1, val2) \
EXPECT_PRED_FORMAT2(embedis_predicates::string, val1, val2)
#define ASSERT_EMBEDIS_STRING(val1, val2) \
ASSERT_PRED_FORMAT2(embedis_predicates::string, val1, val2)

#define EXPECT_EMBEDIS_ARRAY(val1, val2) \
EXPECT_PRED_FORMAT2(embedis_predicates::array, val1, val2)
#define ASSERT_EMBEDIS_ARRAY(val1, val2) \
ASSERT_PRED_FORMAT2(embedis_predicates::array, val1, val2)


#endif // TESTMAIN_H
