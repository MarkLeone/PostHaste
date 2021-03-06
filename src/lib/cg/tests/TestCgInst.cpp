#include "cg/CgInst.h"
#include "cg/CgVars.h"
#include "ir/IRValues.h"
#include "util/UtDelete.h"
#include "util/UtLog.h"
#include <gtest/gtest.h>
#include <iostream>
#include <llvm/BasicBlock.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>

class TestCgInst : public testing::Test { 
public:
    UtLog mLog;
    llvm::LLVMContext mContext;
    IRTypes mTypes;
    CgInst mInsts;
    CgBuilder* mBuilder;
    llvm::Type* mFloatTy;
    IRLocalVar mVar1, mVar2;

    TestCgInst() :
        mLog(stderr),
        mInsts(CgComponent::Create(&mLog, &mContext)),
        mBuilder(mInsts.GetBuilder()),
        mFloatTy(llvm::Type::getFloatTy(*mInsts.GetContext())),
        mVar1("x", mTypes.GetFloatTy(), kIRUniform, ""),
        mVar2("y", mTypes.GetFloatTy(), kIRUniform, "")
    {
        mInsts.InitForTest();
        mInsts.GetVars()->Bind(&mVar1, mBuilder->CreateAlloca(mFloatTy));
        mInsts.GetVars()->Bind(&mVar2, mBuilder->CreateAlloca(mFloatTy));
    }
};

TEST_F(TestCgInst, TestGenInst)
{
    IRValues args;
    args.push_back(&mVar1);
    IRBasicInst inst(kOpcode_Negate, &mVar1, args);

    mInsts.GenInst(inst);
    llvm::outs() << *mBuilder->GetInsertBlock() << "\n";
}

TEST_F(TestCgInst, TestGenAssign)
{
    IRValues args;
    args.push_back(&mVar2);
    IRBasicInst inst(kOpcode_Assign, &mVar1, args);

    mInsts.GenInst(inst);
    llvm::outs() << *mBuilder->GetInsertBlock() << "\n";
    
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
