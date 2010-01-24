#include "ir/IRType.h"
#include "ir/IRTypes.h"
#include "util/UtCast.h"
#include <stdio.h>
#include <gtest/gtest.h>
#include <iostream>

class TestIRType : public testing::Test { };

// ---------- Utilities ----------

// Make a one-member struct.
static const IRStructType*
MakeStruct1(IRTypes* types)
{
    std::vector<const IRType*> memTy;
    std::vector<IRDetail> memDet;
    std::vector<std::string> memName;

    memTy.push_back(types->GetFloatTy());
    memDet.push_back(kIRUniform);
    memName.push_back("x");

    return types->GetStructType("t1", memTy, memDet, memName);
}

// Make a two-member struct.
static const IRStructType*
MakeStruct2(IRTypes* types)
{
    std::vector<const IRType*> memTy;
    std::vector<IRDetail> memDet;
    std::vector<std::string> memName;

    memTy.push_back(types->GetFloatTy());
    memDet.push_back(kIRUniform);
    memName.push_back("x");

    memTy.push_back(types->GetArrayType(types->GetFloatTy(), 3));
    memDet.push_back(kIRVarying);
    memName.push_back("y");

    return types->GetStructType("t2", memTy, memDet, memName);
}

// Make a struct containing a struct and an array of structs.
static const IRStructType*
MakeStruct3(IRTypes* types)
{
    std::vector<const IRType*> memTy;
    std::vector<IRDetail> memDet;
    std::vector<std::string> memName;

    memTy.push_back(MakeStruct1(types));
    memDet.push_back(kIRUniform);
    memName.push_back("a");

    memTy.push_back(types->GetArrayType(MakeStruct2(types), 3));
    memDet.push_back(kIRUniform);
    memName.push_back("b");

    return types->GetStructType("t3", memTy, memDet, memName);
}

// ---------- Type kind testing ----------

TEST_F(TestIRType, TestFloatKind)
{
    IRTypes types;
    const IRType* ty = types.GetFloatTy();

    EXPECT_EQ(IRTypes::GetFloatTy(), ty);
    EXPECT_EQ(kIRFloatTy, ty->GetKind());
    EXPECT_TRUE(ty->IsFloat());
    EXPECT_TRUE(IRType::IsInstance(ty));

    EXPECT_FALSE(ty->IsPoint());
    EXPECT_FALSE(ty->IsArray());
    EXPECT_FALSE(IRArrayType::IsInstance(ty));
}

TEST_F(TestIRType, TestPointKind)
{
    IRTypes types;
    const IRType* ty = types.GetPointTy();

    EXPECT_EQ(IRTypes::GetPointTy(), ty);
    EXPECT_EQ(kIRPointTy, ty->GetKind());
    EXPECT_TRUE(ty->IsPoint());
    EXPECT_TRUE(IRType::IsInstance(ty));

    EXPECT_FALSE(ty->IsFloat());
    EXPECT_FALSE(ty->IsArray());
    EXPECT_FALSE(IRArrayType::IsInstance(ty));
}

TEST_F(TestIRType, TestArrayKind)
{
    IRTypes types;
    const IRType* ty = types.GetArrayType(types.GetFloatTy(), 3);

    EXPECT_EQ(kIRArrayTy, ty->GetKind());
    EXPECT_TRUE(ty->IsArray());
    EXPECT_TRUE(IRType::IsInstance(ty));
    EXPECT_TRUE(IRArrayType::IsInstance(ty));

    EXPECT_FALSE(ty->IsFloat());
    EXPECT_FALSE(IRStructType::IsInstance(ty));
}

TEST_F(TestIRType, TestStructKind)
{
    IRTypes types;
    const IRType* ty = MakeStruct1(&types);

    EXPECT_EQ(kIRStructTy, ty->GetKind());
    EXPECT_TRUE(ty->IsStruct());
    EXPECT_TRUE(IRType::IsInstance(ty));
    EXPECT_TRUE(IRStructType::IsInstance(ty));

    EXPECT_FALSE(ty->IsFloat());
    EXPECT_FALSE(IRArrayType::IsInstance(ty));
}

// ---------- Casting ----------

TEST_F(TestIRType, TestBasicCast)
{
    const IRType* t1 = UtStaticCast<const IRType*>(IRTypes::GetFloatTy());
    const IRType* t2 = UtCast<const IRType*>(IRTypes::GetFloatTy());
    EXPECT_TRUE(t2 != NULL);
}

TEST_F(TestIRType, TestArrayCast)
{
    IRTypes types;
    const IRType* ty = types.GetArrayType(types.GetFloatTy(), 3);
    const IRArrayType* a1 = UtStaticCast<const IRArrayType*>(ty);
    const IRArrayType* a2 = UtCast<const IRArrayType*>(ty);
    EXPECT_TRUE(a2 != NULL);
}

TEST_F(TestIRType, TestStructCast)
{
    IRTypes types;
    const IRType* ty = MakeStruct1(&types);
    const IRStructType* a1 = UtStaticCast<const IRStructType*>(ty);
    const IRStructType* a2 = UtCast<const IRStructType*>(ty);
    EXPECT_TRUE(a2 != NULL);
}

// ---------- Various methods ----------

TEST_F(TestIRType, TestArrayType)
{
    IRTypes types;
    const IRArrayType* ty = types.GetArrayType(types.GetFloatTy(), 3);

    EXPECT_EQ(ty->GetElementType(), types.GetFloatTy());
    EXPECT_EQ(ty->GetLength(), 3);
}

TEST_F(TestIRType, TestStructType)
{
    IRTypes types;
    const IRStructType* ty = MakeStruct2(&types);

    EXPECT_EQ(types.GetFloatTy(), ty->GetMemberType(0));
    EXPECT_EQ(kIRUniform, ty->GetMemberDetail(0));
    EXPECT_STREQ("x", ty->GetMemberName(0));

    EXPECT_EQ(types.GetArrayType(types.GetFloatTy(), 3), ty->GetMemberType(1));
    EXPECT_EQ(kIRVarying, ty->GetMemberDetail(1));
    EXPECT_STREQ("y", ty->GetMemberName(1));
}

// ---------- Output ----------

TEST_F(TestIRType, TestFloatOutput)
{
    std::cout << *IRTypes::GetFloatTy() << std::endl;
}

TEST_F(TestIRType, TestArrayOutput)
{
    IRTypes types;
    const IRType* ty = types.GetArrayType(types.GetFloatTy(), 3);
    std::cout << *ty << std::endl;
}

TEST_F(TestIRType, TestStructOutput)
{
    IRTypes types;
    const IRStructType* ty1 = MakeStruct1(&types);
    const IRStructType* ty2 = MakeStruct2(&types);
    const IRStructType* ty3 = MakeStruct3(&types);
    std::cout << *ty1 << std::endl;
    ty1->Dump(std::cout);
    std::cout << *ty2 << std::endl;
    ty2->Dump(std::cout);
    std::cout << *ty3 << std::endl;
    ty3->Dump(std::cout);
}


int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

