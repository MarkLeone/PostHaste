#include "xf/XfInstrument.h"
#include "ir/IRShader.h"
#include "slo/SloInputFile.h"
#include "slo/SloShader.h"
#include "util/UtLog.h"
#include "xf/XfRaise.h"
#include <gtest/gtest.h>
#include <iostream>

class TestXfInstrument : public testing::Test {
public:
    UtLog mLog;

    TestXfInstrument() : mLog(stderr) { }

    IRShader* LoadShader(const char* filename) 
    {
        SloInputFile in(filename, &mLog);
        int status = in.Open();
        assert(status == 0 && "SLO open failed");
        SloShader slo;
        status = slo.Read(&in);
        assert(status == 0 && "SLO read failed");
        IRShader* shader = XfRaise(slo, &mLog);
        assert(shader != NULL && "Raising to IR failed");
        return shader;
    }

    void TestInstrument(const char* filename) {
        std::cout << "---------- " << filename << " ----------\n";
        IRShader* shader = LoadShader(filename);
        XfInstrument(shader, &mLog, 5);
        std::cout << *shader;
    }
};

TEST_F(TestXfInstrument, TestOnShader) {
    TestInstrument("TestLiveRudyCSkin.slo");
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

