#include "benchtest.hpp"

#ifndef TESTMAIN_H
#define TESTMAIN_H

std::string embedis(std::string cmd);
void fake_eeprom_erase();
testing::AssertionResult embedisOK(const char* cmd_expr, std::string cmd);
testing::AssertionResult embedisFAIL(const char* cmd_expr, std::string cmd);


#define EXPECT_EMBEDIS_OK(val1) \
EXPECT_PRED_FORMAT1(embedisOK, val1)
#define ASSERT_EMBEDIS_OK(val1) \
ASSERT_PRED_FORMAT1(embedisOK, val1)

#define EXPECT_EMBEDIS_FAIL(val1) \
EXPECT_PRED_FORMAT1(embedisFAIL, val1)
#define ASSERT_EMBEDIS_FAIL(val1) \
ASSERT_PRED_FORMAT1(embedisFAIL, val1)


#endif // TESTMAIN_H
