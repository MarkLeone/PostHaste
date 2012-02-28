#include "cg/CgOptimize.h"
#include "cg/CgShader.h"
#include "ir/IRShader.h"
#include "ir/IRValues.h"
#include "slo/SloInputFile.h"
#include "slo/SloShader.h"
#include "util/UtLog.h"
#include "xf/XfRaise.h"
#include <llvm/Function.h>
#include <llvm/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <gtest/gtest.h>
#include <iostream>
#include <stdio.h>

class TestCgShader : public testing::Test {
public:
    UtLog mLog;
    IRTypes mTypes;
    CgShader mCodegen;

    TestCgShader() :
        mLog(stderr),
        mCodegen(&mLog)
    {
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

    void TestShaderCodegen(const char* filename) {
        llvm::outs() << "---------- " << filename << " ----------\n";
        IRShader* shader = LoadShader(filename);
        llvm::Module* module = CgShaderCodegen(shader, &mLog, 1, true);
        ASSERT_TRUE(module != NULL);
        // Eliminte unused functions and constants.
        CgOptimize(module, 0);
        // Print non-static globals and functions.
        const llvm::Module::GlobalListType& globals =
            module->getGlobalList();
        llvm::Module::GlobalListType::const_iterator global;
        for (global = globals.begin(); global != globals.end(); ++global) {
            if (!global->hasInternalLinkage())
                llvm::outs() << *global;
        }
        const llvm::Module::FunctionListType& funcs =
            module->getFunctionList();
        llvm::Module::FunctionListType::const_iterator func;
        for (func = funcs.begin(); func != funcs.end(); ++func) {
            if (!func->hasInternalLinkage())
                llvm::outs() << *func;
        }
    }
};

TEST_F(TestCgShader, TestGenPrototype1)
{
    IRLocalVar x1("x1", mTypes.GetFloatTy(), kIRUniform, "");
    IRLocalVar x2("x2", mTypes.GetPointTy(), kIRVarying, "");
    const IRType* arrayTy = mTypes.GetArrayType(mTypes.GetFloatTy(), 3);
    IRLocalVar x3("x3", arrayTy, kIRVarying, "");
    IRGlobalVar P("P", mTypes.GetPointTy(), kIRVarying, NULL, false);
    IRShaderParam param("param", mTypes.GetFloatTy(),
                        kIRUniform, "", false, new IRSeq());
    IRVars vars(5, &x1, &x2, &x3, &P, &param);
    std::string proto = mCodegen.GenPrototype("func", vars);
    llvm::outs() << "TestGenPrototype1:\n" <<  proto << "\n";
}

TEST_F(TestCgShader, TestGenPrototype2)
{
    const char* filename = "TestCgShader2.slo";
    IRShader* shader = LoadShader(filename);
    IRVars vars;
    vars.insert(vars.end(),
                shader->GetParams().begin(), shader->GetParams().end());
    vars.insert(vars.end(),
                shader->GetGlobals().begin(), shader->GetGlobals().end());
    llvm::outs() << "TestGenPrototype2:\n"
              << mCodegen.GenPrototype("func", vars) << "\n";
}

TEST_F(TestCgShader, TestGenKernelFunc)
{
    const char* filename = "TestCgShader2.slo";
    IRShader* shader = LoadShader(filename);
    IRVars vars;
    vars.insert(vars.end(),
                shader->GetParams().begin(), shader->GetParams().end());
    vars.insert(vars.end(),
                shader->GetGlobals().begin(), shader->GetGlobals().end());
    llvm::Function* function = mCodegen.GenKernelFunc("func", vars);
    llvm::outs() << "TestGenKernelFunc:\n" << *function;
}

TEST_F(TestCgShader, TestGenPluginCall)
{
    const char* filename = "TestCgShader2.slo";
    IRShader* shader = LoadShader(filename);
    IRVars vars;
    vars.insert(vars.end(),
                shader->GetParams().begin(), shader->GetParams().end());
    vars.insert(vars.end(),
                shader->GetGlobals().begin(), shader->GetGlobals().end());
    mCodegen.CodegenSetup(shader);
    std::string protoStr = mCodegen.GenPrototype("func", vars);
    IRStringConst* prototype = shader->NewStringConst(protoStr.c_str());
    IRStmt* call = mCodegen.GenPluginCall("func", vars, prototype, IRPos());
    std::cout << "TestGenPluginCall:\n" << *call;
    delete shader;
}

TEST_F(TestCgShader, TestCodegenPartition)
{
    const char* filename = "TestCgShader1.slo";
    IRShader* shader = LoadShader(filename);
    mCodegen.CodegenSetup(shader);
    ASSERT_TRUE(shader->GetBody()->CanCompile());
    shader->SetBody( mCodegen.CodegenPartition(shader->GetBody()));
    std::cout << "TestCodegenParition:\n" << *shader->GetBody();
    llvm::outs() << *mCodegen.GetModule()->getFunction("TestCgShader1");
    delete shader;
}

TEST_F(TestCgShader, TestShaders)
{
    TestShaderCodegen("TestCgShader1.slo");
    TestShaderCodegen("TestCgShader2.slo");
    TestShaderCodegen("TestRudyCSkin.slo");
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
