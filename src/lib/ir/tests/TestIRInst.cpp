#include "ir/IRBasicInst.h"
#include "ir/IRValues.h"
#include "util/UtCast.h"
#include <gtest/gtest.h>
#include <iostream>

class TestIRInst : public testing::Test { };

TEST_F(TestIRInst, TestBasicInst)
{
    IRTypes types;
    IRLocalVar var("x", types.GetFloatTy(), kIRUniform, ""); 
    IRValues args;
    args.push_back(&var);
    IRInst* inst = new IRBasicInst(kOpcode_Negate, &var, args);
    std::cout << *inst << std::endl;

    EXPECT_EQ(kIRBasicInst, inst->GetKind());
    EXPECT_TRUE(IRBasicInst::IsInstance(inst));
    EXPECT_TRUE(IRInst::IsInstance(inst));
    EXPECT_TRUE(IRValue::IsInstance(inst));
    EXPECT_FALSE(IRUnknownInst::IsInstance(inst));

    const IRInst* i1 = UtStaticCast<const IRInst*>(inst);
    const IRInst* i2 = UtCast<const IRInst*>(inst);
    EXPECT_TRUE(i2 != NULL);
    const IRInst* i3 = UtCast<const IRUnknownInst*>(inst);
    EXPECT_TRUE(i3 == NULL);
    delete inst;
}


int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

