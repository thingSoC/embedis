#include <benchtest.hpp>
#include "embedis.h"

int main() {
    testing::reporter(new testing::DefaultReporter);
    return testing::run();
}

static std::string result;

extern "C" void embedis_out(char data) {
    result.append(1, data);
}

static std::string embedis(const char* cmd) {
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



TEST(Protocol, WithExtraSapces){

    embedis_reset();

    EXPECT_EQ(
              embedis("GET vendor"),
              "+AE9RB\r\n"
              );

    EXPECT_EQ(
              embedis("  GET vendor"),
              "+AE9RB\r\n"
              );

    EXPECT_EQ(
              embedis("GET   vendor"),
              "+AE9RB\r\n"
              );

    EXPECT_EQ(
              embedis("GET vendor  "),
              "+AE9RB\r\n"
              );

}


TEST(Protocol, Caps){

    embedis_reset();

    EXPECT_EQ(
              embedis("get vendor"),
              "+AE9RB\r\n"
              );

    EXPECT_EQ(
              embedis("GET vendor"),
              "+AE9RB\r\n"
              );

    EXPECT_EQ(
              embedis("GeT vendor"),
              "+AE9RB\r\n"
              );

}

TEST(Protocol, Overflow){

    embedis_reset();

    std::string s;

    // The 1 is for the trailing zero
    s.append(EMBEDIS_COMMAND_BUF_SIZE-1, 'X');
    EXPECT_EQ(
              embedis(s.c_str()),
              "-ERROR unknown command\r\n"
              );

    // This one should overflow
    s.append(1, 'X');
    EXPECT_EQ(
              embedis(s.c_str()),
              "-ERROR buffer overflow\r\n"
              );

    s.clear();
    for (int i = 0; i < EMBEDIS_COMMAND_MAX_ARGS; i++) {
        s.append("Z ");
    }
    EXPECT_EQ(
              embedis(s.c_str()),
              "-ERROR unknown command\r\n"
              );

    s.append("Z ");
    EXPECT_EQ(
              embedis(s.c_str()),
              "-ERROR bad argument count\r\n"
              );

    s.append("Z Z Z Z");
    EXPECT_EQ(
              embedis(s.c_str()),
              "-ERROR bad argument count\r\n"
              );


}


TEST(DictROM, Basics){

    embedis_reset();

    EXPECT_EQ(
              embedis("GET vendor"),
              "+AE9RB\r\n"
              ) << "Retrieve ROM value";;

    EXPECT_EQ(
              embedis("SET vendor blah"),
              "-ERROR\r\n"
              ) << "ROM values are read-only";

    EXPECT_EQ(
              embedis("DEL vendor"),
              "-ERROR\r\n"
              ) << "ROM values are read-only";


}

TEST(DictNVRAM, Basics){

    embedis_reset();

    EXPECT_EQ(
              embedis("SET foo1 bar1"),
              "+OK\r\n"
              );

    EXPECT_EQ(
              embedis("SET foo bar"),
              "+OK\r\n"
              );

    EXPECT_EQ(
              embedis("SET foo2 bar2"),
              "+OK\r\n"
              );

    EXPECT_EQ(
              embedis("SET foo good"),
              "+OK\r\n"
              );

    EXPECT_EQ(
              embedis("SET foo3 bar3"),
              "+OK\r\n"
              );

    EXPECT_EQ(
              embedis("GET foo"),
              "$4\r\ngood\r\n"
              );

    EXPECT_EQ(
              embedis("DEL foo"),
              "+OK\r\n"
              );

    EXPECT_EQ(
              embedis("GET foo"),
              "$-1\r\n"
              );

    EXPECT_EQ(
              embedis("GET foo1"),
              "$4\r\nbar1\r\n"
              );

    EXPECT_EQ(
              embedis("GET foo2"),
              "$4\r\nbar2\r\n"
              );

    EXPECT_EQ(
              embedis("GET foo3"),
              "$4\r\nbar3\r\n"
              );


}
