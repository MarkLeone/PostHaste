#include "ops/OpcodeNames.h"
#include "slo/SloInputFile.h"
#include "slo/SloOutputFile.h"
#include "slo/SloShader.h"
#include "util/UtLog.h"
#include <stdio.h>
#include <gtest/gtest.h>

class TestSloRead : public testing::Test { };

TEST_F(TestSloRead, TestCopy)
{
    // A fake slo without some sections is used (no strings, no constants).
    UtLog log(stderr);
    SloInputFile in("fake.slo", &log);
    SloOutputFile out("fake_copy.slo", &log);
    ASSERT_EQ(0, in.Open()); 
    ASSERT_EQ(0, out.Open()); 

    SloHeader header;
    ASSERT_EQ(0, header.Read(&in));
    header.Write(&out);

    SloInfo info;
    ASSERT_EQ(0, info.Read(&in));
    info.Write(&out);

    int numSymbols;
    const char* line = in.ReadLine();
    ASSERT_TRUE(line != NULL);
    ASSERT_EQ(1, sscanf(line, "%i", &numSymbols));
    out.Write("%i\n", numSymbols);

    SloSymbol symbol;
    for (int i = 0; i < numSymbols; ++i) {
        ASSERT_EQ(0, symbol.Read(&in));
        symbol.Write(&out);
    }

    SloInst instr;
    OpcodeNames opNames;
    for (int i = 0; i < info.mNumInsts; ++i) {
        ASSERT_EQ(0, instr.Read(&in, &opNames));
        instr.Write(&out);
    }
}

TEST_F(TestSloRead, TestShaderCopy)
{
    UtLog log(stderr);
    SloInputFile in("dented.slo", &log);
    SloOutputFile out("dented_copy.slo", &log);
    ASSERT_EQ(0, in.Open()); 
    ASSERT_EQ(0, out.Open()); 

    SloShader shader;
    ASSERT_EQ(0, shader.Read(&in));
    shader.Write(&out);
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
