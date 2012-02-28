#include "cg/CgConst.h"
#include "cg/CgTypes.h"
#include "ir/IRValues.h"
#include "ir/IRTypes.h"
#include "util/UtLog.h"
#include <gtest/gtest.h>
#include <iostream>
#include <llvm/Constants.h>
#include <llvm/DerivedTypes.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Support/raw_ostream.h>

class TestCgConst : public testing::Test {
public:
    UtLog mLog;
    llvm::LLVMContext mContext;
    CgConst mCodegen;
    IRTypes mTypes;

    TestCgConst() :
        mLog(stderr),
        mCodegen(CgComponent::Create(&mLog, &mContext))
    {
        mCodegen.InitForTest();
    }

    ~TestCgConst() {
        mCodegen.Destroy();
    }
};

TEST_F(TestCgConst, TestFloatConst)
{
    IRNumConst v(1.0f);
    llvm::Constant* c = mCodegen.Convert(&v);
    llvm::outs() << *c << "\n";

    const llvm::ConstantFP* f = llvm::dyn_cast<llvm::ConstantFP>(c);
    ASSERT_TRUE(f != NULL);
    EXPECT_TRUE(f->isExactlyValue(1.0f));
}

TEST_F(TestCgConst, TestTripleConst)
{
    const float data[] = { 1.0f, 2.0f, 3.0f };
    IRNumConst v(data, mTypes.GetPointTy());

    llvm::Constant* c = mCodegen.Convert(&v);
    llvm::outs() << *c << "\n";
    EXPECT_TRUE(llvm::isa<llvm::ConstantStruct>(c));
}

TEST_F(TestCgConst, TestArrayConst)
{
    const IRArrayType* ty = mTypes.GetArrayType(mTypes.GetFloatTy(), 3);
    const float elements[] = { 1.0f, 2.0f, 3.0f };
    IRNumArrayConst v(elements, ty);

    llvm::Constant* c = mCodegen.Convert(&v);
    llvm::outs() << *c << "\n";
    EXPECT_TRUE(llvm::isa<llvm::ConstantArray>(c));
}

TEST_F(TestCgConst, TestTripleArrayConst)
{
    const IRArrayType* ty = mTypes.GetArrayType(mTypes.GetPointTy(), 2);
    const float elements[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f };
    IRNumArrayConst v(elements, ty);

    llvm::Constant* c = mCodegen.Convert(&v);
    llvm::outs() << *c << "\n";
    EXPECT_TRUE(llvm::isa<llvm::ConstantArray>(c));
}

TEST_F(TestCgConst, TestStringConst)
{
    IRStringConst v("hello");

    llvm::Constant* c = mCodegen.Convert(&v);
    llvm::outs() << *c << "\n";
    EXPECT_TRUE(llvm::isa<llvm::ConstantExpr>(c));
}

TEST_F(TestCgConst, TestStringArrayConst)
{
    const IRArrayType* ty = mTypes.GetArrayType(mTypes.GetStringTy(), 3);
    std::vector<std::string> elements;
    elements.push_back("one");
    elements.push_back("two");
    elements.push_back("three");
    IRStringArrayConst v(&elements[0], ty);

    llvm::Constant* c = mCodegen.Convert(&v);
    llvm::outs() << *c << "\n";
    EXPECT_TRUE(llvm::isa<llvm::ConstantArray>(c));
}

TEST_F(TestCgConst, TestMatrixConst)
{
    const float data[] = { 1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16 };
    IRNumConst v(data, mTypes.GetMatrixTy());

    llvm::Constant* c = mCodegen.Convert(&v);
    // llvm::outs() << *c << "\n";
    llvm::outs() << *c->getType() << "\n";
    EXPECT_TRUE(llvm::isa<llvm::ConstantStruct>(c));
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
