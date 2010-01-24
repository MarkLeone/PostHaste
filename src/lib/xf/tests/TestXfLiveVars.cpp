#include "ir/IRInst.h"
#include "ir/IRShader.h"
#include "ir/IRValues.h"
#include "ir/IRVisitor.h"
#include "slo/SloInputFile.h"
#include "slo/SloShader.h"
#include "util/UtLog.h"
#include "xf/XfLiveVars.h"
#include "xf/XfRaise.h"
#include <gtest/gtest.h>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>

class TestXfLiveVars : public testing::Test { 
public:
    UtLog mLog;
    IRTypes mTypes;
    IRLocalVar *x1, *x2, *x3, *x4; // pointers make tests more legible.
    IRGlobalVars mNoGlobals;

    TestXfLiveVars() :
        mLog(stderr),
        // The types of the variables don't matter in this test.
        x1(new IRLocalVar("x1", mTypes.GetFloatTy(), kIRUniform, "")),
        x2(new IRLocalVar("x2", mTypes.GetFloatTy(), kIRUniform, "")),
        x3(new IRLocalVar("x3", mTypes.GetFloatTy(), kIRUniform, "")),
        x4(new IRLocalVar("x4", mTypes.GetFloatTy(), kIRUniform, ""))
    {
    }

    ~TestXfLiveVars() 
    {
        delete x1; delete x2; delete x3; delete x4;
    }


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

    void TestShader(const char* filename)
    {
        // Print the variables that are live at the start of the shader.  Note
        // that we don't seed the live variable set with any shader outputs,
        // so they're not necessarily live unless they're read.
        IRShader* shader = LoadShader(filename);
        IRVarSet live;
        XfLiveVarsImpl liveVars(shader->GetGlobals());
        liveVars.GetLive(shader->GetBody(), &live);
        std::cout << filename << ": " << live << std::endl;
    }
};

TEST_F(TestXfLiveVars, TestAssign)
{
    IRBasicInst inst(kOpcode_Assign, x1, IRValues(2, x2, x3));
    IRVarSet live;
    XfLiveVarsImpl(mNoGlobals).GetLive(&inst, &live);
    EXPECT_TRUE(live.Has(x2) && live.Has(x3));
    EXPECT_FALSE(live.Has(x1));
    std::cout << "TestAssign: " << live << std::endl;
}

TEST_F(TestXfLiveVars, TestArrayAssign)
{
    // Types of arguments don't matter, so we play it loose.
    IRBasicInst inst(kOpcode_ArrayAssign, NULL, IRValues(3, x1, x2, x3));
    IRVarSet live;
    XfLiveVarsImpl(mNoGlobals).GetLive(&inst, &live);
    EXPECT_TRUE(live.Has(x1));
    std::cout << "TestArrayAssign: " << live << std::endl;
}

TEST_F(TestXfLiveVars, TestArgKill)
{
    // Actually, there aren't any shadeops that kill their output arguments
    // (see OpInfo::KillsArg).  In this case, lightsource() doesn't
    // kill its output because the specified name might not exit.
    IRBasicInst inst(kOpcode_LightSource, x1, IRValues(2, x2, x3));
    IRVarSet live;
    XfLiveVarsImpl(mNoGlobals).GetLive(&inst, &live);
    EXPECT_FALSE(live.Has(x1));
    EXPECT_TRUE(live.Has(x2));
    EXPECT_TRUE(live.Has(x3));  // note: the output, x3, is not killed!
    std::cout << "TestArgKill: " << live << std::endl;
}

TEST_F(TestXfLiveVars, TestAssignKill)
{
    // x1 is live in inst2, but killed by inst1.
    IRBasicInst inst1(kOpcode_Assign, x1, IRValues(1, x2));
    IRBasicInst inst2(kOpcode_Assign, x3, IRValues(1, x1));
    IRVarSet live;
    XfLiveVarsImpl(mNoGlobals).GetLive(&inst2, &live);
    XfLiveVarsImpl(mNoGlobals).GetLive(&inst1, &live);
    EXPECT_FALSE(live.Has(x1));
    EXPECT_TRUE(live.Has(x2));
    EXPECT_FALSE(live.Has(x3));
    std::cout << "TestAssignKill: " << live << std::endl;
}

TEST_F(TestXfLiveVars, TestBlock)
{
    // x1 is live in inst2, but killed by inst1.
    IRInst* inst1 = new IRBasicInst(kOpcode_Assign, x1, IRValues(1, x2));
    IRInst* inst2 = new IRBasicInst(kOpcode_Assign, x3, IRValues(1, x1));
    IRBlock block(new IRInsts(2, inst1, inst2));
    IRVarSet live;
    XfLiveVarsImpl(mNoGlobals).Visit(&block, &live);
    EXPECT_FALSE(live.Has(x1));
    EXPECT_TRUE(live.Has(x2));
    EXPECT_FALSE(live.Has(x3));
    std::cout << "TestBlock: " << live << std::endl;
}

TEST_F(TestXfLiveVars, TestSeq)
{
    IRInst* inst1 = new IRBasicInst(kOpcode_Assign, x1, IRValues(1, x2));
    IRInst* inst2 = new IRBasicInst(kOpcode_Assign, x3, IRValues(1, x1));
    IRBlock* block1 = new IRBlock(new IRInsts(1, inst1));
    IRBlock* block2 = new IRBlock(new IRInsts(1, inst2));
    IRSeq seq(new IRStmts(2, block1, block2));
    IRVarSet live;
    XfLiveVarsImpl(mNoGlobals).Visit(&seq, &live);
    EXPECT_FALSE(live.Has(x1));
    EXPECT_TRUE(live.Has(x2));
    EXPECT_FALSE(live.Has(x3));
    std::cout << "TestSeq: " << live << std::endl;
}

TEST_F(TestXfLiveVars, TestShaders)
{
    TestShader("TestLive1.slo");
    TestShader("TestLive2.slo");
    TestShader("TestLive3.slo");
    TestShader("TestLive4.slo");
    TestShader("TestLiveLoop1.slo");
    TestShader("TestLiveLoop2.slo");
    TestShader("TestLiveLoop3.slo");
    TestShader("TestLiveLoop4.slo");
    TestShader("TestLiveLoop5.slo");
    TestShader("TestLiveLoop6.slo");
    TestShader("TestLiveLoop7.slo");
    TestShader("TestLiveLoop8.slo");
    TestShader("TestLiveLoop9.slo");
    TestShader("TestLiveLoop10.slo");
    TestShader("TestLiveIllum1.slo");
    TestShader("TestLiveIllum2.slo");
    TestShader("TestLiveIllum3.slo");
    TestShader("TestLiveIllum4.slo");
    TestShader("TestLiveIllum5.slo");
    TestShader("TestLiveIllum6.slo");
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

