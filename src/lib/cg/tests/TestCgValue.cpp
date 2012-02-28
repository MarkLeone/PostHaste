#include "cg/CgValue.h"
#include "cg/CgTypes.h"
#include "ir/IRValues.h"
#include "ir/IRTypes.h"
#include "util/UtLog.h"
#include <gtest/gtest.h>
#include <iostream>
#include <llvm/Constants.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Support/raw_ostream.h>

class TestCgValue : public testing::Test { 
public:
    UtLog mLog;
    CgValue mCodegen;
    IRTypes mTypes;

    TestCgValue() :
        mLog(stderr),
        mCodegen(CgComponent::Create(&mLog))
    {
        mCodegen.InitForTest();
    }

    ~TestCgValue() {
        mCodegen.Destroy();
    }

};

TEST_F(TestCgValue, TestFloatConst)
{
    IRNumConst v(1.0f);
    llvm::Value* arg = mCodegen.ConvertArg(&v);
    llvm::outs() << *arg << "\n";
    EXPECT_TRUE(llvm::isa<llvm::ConstantFP>(arg));
}

TEST_F(TestCgValue, TestTripleConst)
{
    const float data[] = { 1.0f, 2.0f, 3.0f };
    IRNumConst v(data, mTypes.GetPointTy());

    llvm::Value* arg = mCodegen.ConvertArg(&v);
    llvm::outs() << *arg << "\n";
}

TEST_F(TestCgValue, TestArrayConst)
{
    const IRArrayType* ty = mTypes.GetArrayType(mTypes.GetFloatTy(), 3);
    const float elements[] = { 1.0f, 2.0f, 3.0f };
    IRNumArrayConst v(elements, ty);

    llvm::Value* arg = mCodegen.ConvertArg(&v);
    llvm::outs() << *arg << "\n";
}

TEST_F(TestCgValue, TestTripleArrayConst)
{
    const IRArrayType* ty = mTypes.GetArrayType(mTypes.GetPointTy(), 2);
    const float elements[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f };
    IRNumArrayConst v(elements, ty);

    llvm::Value* arg = mCodegen.ConvertArg(&v);
    llvm::outs() << *arg << "\n";
}

TEST_F(TestCgValue, TestStringConst)
{
    IRStringConst v("hello");

    llvm::Value* arg = mCodegen.ConvertArg(&v);
    llvm::outs() << *arg << "\n";
    EXPECT_TRUE(llvm::isa<llvm::ConstantExpr>(arg));
}

TEST_F(TestCgValue, TestStringArrayConst)
{
    const IRArrayType* ty = mTypes.GetArrayType(mTypes.GetStringTy(), 3);
    std::vector<std::string> elements;
    elements.push_back("one");
    elements.push_back("two");
    elements.push_back("three");
    IRStringArrayConst v(&elements[0], ty);

    llvm::Value* arg = mCodegen.ConvertArg(&v);
    llvm::outs() << *arg << "\n";
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
