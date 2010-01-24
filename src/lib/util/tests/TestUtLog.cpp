#include "util/UtLog.h"
#include <stdio.h>

#include <gtest/gtest.h>

class TestUtLog : public testing::Test { };

TEST_F(TestUtLog, TestInfo)
{
    UtLog(stdout).Write(kUtInfo, "Test: %s", "hello, world!");
}

TEST_F(TestUtLog, TestWarning)
{
    UtLog(stderr).WriteWhere(kUtWarning, "test.txt", 7, "Test: %i", 42);
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
