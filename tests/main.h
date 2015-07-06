#include "benchtest.hpp"

#ifndef TESTMAIN_H
#define TESTMAIN_H

std::string embedis(std::string cmd);
void fake_eeprom_erase();
testing::AssertionResult embedisOK(const char* cmd_expr, std::string cmd);
testing::AssertionResult embedisFAIL(const char* cmd_expr, std::string cmd);

#endif // TESTMAIN_H
