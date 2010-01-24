#include "ir/IRValues.h"
#include "ir/IRTypes.h"
#include "util/UtCast.h"
#include <stdio.h>
#include <gtest/gtest.h>
#include <iostream>

class TestIRConst : public testing::Test { };

// ---------- Value kind testing ----------

TEST_F(TestIRConst, TestFloatConst)
{
    IRNumConst v(1.0f);
    ASSERT_TRUE(v.IsFloat());
    EXPECT_EQ(1.0f, v.GetFloat());

    EXPECT_EQ(kIRNumConst, v.GetKind());
    EXPECT_TRUE(v.IsConst());
    EXPECT_TRUE(IRNumConst::IsInstance(&v));
    EXPECT_TRUE(IRConst::IsInstance(&v));
    EXPECT_TRUE(IRValue::IsInstance(&v));

    EXPECT_FALSE(IRStringConst::IsInstance(&v));
    EXPECT_FALSE(v.IsVar());
    EXPECT_FALSE(v.IsInst());
    EXPECT_FALSE(IRVar::IsInstance(&v));
    EXPECT_FALSE(IRInst::IsInstance(&v));

    std::cout << v << std::endl;
}

TEST_F(TestIRConst, TestStringConst)
{
    IRStringConst v("hello");
    EXPECT_EQ(std::string("hello"), v.Get());

    EXPECT_EQ(kIRStringConst, v.GetKind());
    EXPECT_TRUE(v.IsConst());
    EXPECT_TRUE(IRStringConst::IsInstance(&v));
    EXPECT_TRUE(IRConst::IsInstance(&v));
    EXPECT_TRUE(IRValue::IsInstance(&v));

    EXPECT_FALSE(IRNumConst::IsInstance(&v));
    EXPECT_FALSE(v.IsVar());
    EXPECT_FALSE(v.IsInst());
    EXPECT_FALSE(IRVar::IsInstance(&v));
    EXPECT_FALSE(IRInst::IsInstance(&v));

    std::cout << v << std::endl;
}

TEST_F(TestIRConst, TestTripleConst)
{
    IRTypes types;
    const float data[] = { 1.0f, 2.0f, 3.0f };
    IRNumConst v(data, types.GetPointTy());
    const float* got = v.GetData();
    EXPECT_EQ(1, got[0]);
    EXPECT_EQ(2, got[1]);
    EXPECT_EQ(3, got[2]);

    EXPECT_EQ(kIRNumConst, v.GetKind());
    EXPECT_TRUE(v.IsConst());
    EXPECT_TRUE(IRNumConst::IsInstance(&v));
    EXPECT_TRUE(IRConst::IsInstance(&v));
    EXPECT_TRUE(IRValue::IsInstance(&v));

    EXPECT_FALSE(IRNumArrayConst::IsInstance(&v));
    EXPECT_FALSE(v.IsVar());
    EXPECT_FALSE(v.IsInst());
    EXPECT_FALSE(IRVar::IsInstance(&v));
    EXPECT_FALSE(IRInst::IsInstance(&v));

    std::cout << v << std::endl;
}

TEST_F(TestIRConst, TestArrayConst)
{
    IRTypes types;
    const IRArrayType* ty = types.GetArrayType(types.GetFloatTy(), 3);
    const float elements[] = { 1.0f, 2.0f, 3.0f };
    IRNumArrayConst v(elements, ty);

    EXPECT_EQ(1, *v.GetElement(0));
    EXPECT_EQ(2, *v.GetElement(1));
    EXPECT_EQ(3, *v.GetElement(2));

    const float* got = v.GetData();
    EXPECT_EQ(1, got[0]);
    EXPECT_EQ(2, got[1]);
    EXPECT_EQ(3, got[2]);

    EXPECT_EQ(kIRNumArrayConst, v.GetKind());
    EXPECT_TRUE(v.IsConst());
    EXPECT_TRUE(IRNumArrayConst::IsInstance(&v));
    EXPECT_TRUE(IRConst::IsInstance(&v));
    EXPECT_TRUE(IRValue::IsInstance(&v));

    EXPECT_FALSE(IRNumConst::IsInstance(&v));
    EXPECT_FALSE(v.IsVar());
    EXPECT_FALSE(v.IsInst());
    EXPECT_FALSE(IRVar::IsInstance(&v));
    EXPECT_FALSE(IRInst::IsInstance(&v));

    std::cout << v << std::endl;
}

TEST_F(TestIRConst, TestStringArrayConst)
{
    IRTypes types;
    const IRArrayType* ty = types.GetArrayType(types.GetStringTy(), 3);
    std::vector<std::string> elements;
    elements.push_back("one");
    elements.push_back("two");
    elements.push_back("three");
    IRStringArrayConst v(&elements[0], ty);

    EXPECT_STREQ("one", v.GetElement(0));
    EXPECT_STREQ("two", v.GetElement(1));
    EXPECT_STREQ("three", v.GetElement(2));

    EXPECT_EQ(kIRStringArrayConst, v.GetKind());
    EXPECT_TRUE(v.IsConst());
    EXPECT_TRUE(IRStringArrayConst::IsInstance(&v));
    EXPECT_TRUE(IRConst::IsInstance(&v));
    EXPECT_TRUE(IRValue::IsInstance(&v));

    EXPECT_FALSE(IRNumConst::IsInstance(&v));
    EXPECT_FALSE(v.IsVar());
    EXPECT_FALSE(v.IsInst());
    EXPECT_FALSE(IRVar::IsInstance(&v));
    EXPECT_FALSE(IRInst::IsInstance(&v));

    std::cout << v << std::endl;
}

// ---------- Casting ----------

TEST_F(TestIRConst, TestBasicCast)
{
    IRNumConst v(1.0f);
    const IRConst* t1 = UtStaticCast<const IRConst*>(&v);
    const IRConst* t2 = UtCast<const IRConst*>(&v);
    EXPECT_TRUE(t2 != NULL);
}

TEST_F(TestIRConst, TestArrayCast)
{
    IRTypes types;
    const IRArrayType* ty = types.GetArrayType(types.GetFloatTy(), 3);
    const float elements[] = { 1.0f, 2.0f, 3.0f };
    IRNumArrayConst v(elements, ty);

    const IRNumArrayConst* a1 = UtStaticCast<const IRNumArrayConst*>(&v);
    const IRNumArrayConst* a2 = UtCast<const IRNumArrayConst*>(&v);
    EXPECT_TRUE(a2 != NULL);
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

