#include "ir/IRValues.h"
#include "ir/IRTypes.h"
#include "util/UtCast.h"
#include <stdio.h>
#include <gtest/gtest.h>
#include <iostream>

class TestIRVar : public testing::Test { };

TEST_F(TestIRVar, TestGlobalVar)
{
    IRTypes types;
    IRGlobalVar v("P", types.GetPointTy(), kIRVarying, "world", false);

    EXPECT_STREQ("P", v.GetShortName());
    EXPECT_EQ(types.GetPointTy(), v.GetType());
    EXPECT_EQ(kIRVarying, v.GetDetail());
    EXPECT_STREQ("world", v.GetSpace());

    EXPECT_EQ(kIRGlobalVar, v.GetKind());
    EXPECT_TRUE(v.IsVar());
    EXPECT_TRUE(IRGlobalVar::IsInstance(&v));
    EXPECT_TRUE(IRVar::IsInstance(&v));
    EXPECT_TRUE(IRValue::IsInstance(&v));

    EXPECT_FALSE(v.IsConst());
    EXPECT_FALSE(v.IsInst());
    EXPECT_FALSE(IRConst::IsInstance(&v));
    EXPECT_FALSE(IRInst::IsInstance(&v));

    std::cout << v << std::endl;
}

// ---------- Casting ----------

TEST_F(TestIRVar, TestGlobalCast)
{
    IRTypes types;
    IRGlobalVar v("P", types.GetPointTy(), kIRVarying, "world", false);
    
    const IRVar* v1 = UtStaticCast<const IRVar*>(&v);
    const IRVar* v2 = UtCast<const IRVar*>(&v);
    EXPECT_TRUE(v2 != NULL);

    const IRVar* v3 = UtStaticCast<const IRVar*>(v1);
    const IRVar* v4 = UtCast<const IRVar*>(v2);
    EXPECT_TRUE(v4 != NULL);
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

