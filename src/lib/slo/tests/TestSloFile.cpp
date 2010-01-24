#include "slo/SloInputFile.h"
#include "slo/SloOutputFile.h"
#include "util/UtLog.h"
#include <stdio.h>
#include <gtest/gtest.h>

class TestSloFile : public testing::Test { };

TEST_F(TestSloFile, TestCtor)
{
    UtLog log(stderr);
    SloInputFile file("none.slo", &log);
    EXPECT_EQ(0U, file.GetLineNum());
}

TEST_F(TestSloFile, TestOpenRead)
{
    UtLog log(stderr);
    SloInputFile file("dented.slo", &log);
    ASSERT_EQ(0, file.Open());
}

TEST_F(TestSloFile, TestReadLines)
{
    UtLog log(stderr);
    SloInputFile file("dented.slo", &log);
    ASSERT_EQ(0, file.Open());
    EXPECT_EQ(0U, file.GetLineNum());

    const char *line;
    do {
        line = file.ReadLine();
    } while (line != NULL);

    // The "dented.slo" test file has 83 lines.
    file.Report(kUtInfo, "Read %i lines", file.GetLineNum());
    EXPECT_EQ(83U, file.GetLineNum());
}

TEST_F(TestSloFile, TestOpenWrite)
{
    UtLog log(stderr);
    SloOutputFile file("temp.slo", &log);
    ASSERT_EQ(file.Open(), 0);
}
// TODO: test opening existing/read-only files.

TEST_F(TestSloFile, TestWrite)
{
    UtLog log(stderr);
    SloOutputFile file("temp.slo", &log);
    ASSERT_EQ(file.Open(), 0);
    file.Write("shader_obj %.1f %i %i\n", 3.1f, 1, 0);
}


int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
