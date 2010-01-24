#include "ir/IRTypes.h"
#include "ir/IRArrayType.h"
#include "ir/IRStructType.h"
#include <stdio.h>
#include <gtest/gtest.h>

class TestIRTypesFactory : public testing::Test { };

TEST_F(TestIRTypesFactory, TestBasicType)
{
    ASSERT_TRUE(IRTypes::GetFloatTy() != NULL);
}

TEST_F(TestIRTypesFactory, TestBasicArrays)
{
    IRTypes types;
    const IRArrayType* t1 = types.GetArrayType(types.GetFloatTy(), 3);
    const IRArrayType* t2 = types.GetArrayType(types.GetFloatTy(), 3);
    const IRArrayType* t3 = types.GetArrayType(types.GetFloatTy(), 4);
    const IRArrayType* t4 = types.GetArrayType(types.GetFloatTy(), -1);
    const IRArrayType* t5 = types.GetArrayType(types.GetPointTy(), 3);

    EXPECT_EQ(t1, t2);
    EXPECT_NE(t1, t3);
    EXPECT_NE(t1, t4);
    EXPECT_NE(t1, t5);

    EXPECT_NE(t2, t3);
    EXPECT_NE(t2, t4);
    EXPECT_NE(t2, t5);

    EXPECT_NE(t3, t4);
    EXPECT_NE(t3, t5);

    EXPECT_NE(t4, t5);
}

TEST_F(TestIRTypesFactory, TestNestedArrays)
{
    IRTypes types;
    const IRArrayType* e1 = types.GetArrayType(types.GetFloatTy(), 3);
    const IRArrayType* e2 = types.GetArrayType(types.GetFloatTy(), 4);
    const IRArrayType* e3 = types.GetArrayType(types.GetPointTy(), 3);

    const IRArrayType* t1 = types.GetArrayType(e1, 3);
    const IRArrayType* t2 = types.GetArrayType(e1, 3);
    const IRArrayType* t3 = types.GetArrayType(e1, 4);
    const IRArrayType* t4 = types.GetArrayType(e2, 3);
    const IRArrayType* t5 = types.GetArrayType(e3, 3);

    EXPECT_EQ(t1, t2);
    EXPECT_NE(t1, t3);
    EXPECT_NE(t1, t4);
    EXPECT_NE(t1, t5);

    EXPECT_NE(t2, t3);
    EXPECT_NE(t2, t4);
    EXPECT_NE(t2, t5);

    EXPECT_NE(t3, t4);
    EXPECT_NE(t3, t5);

    EXPECT_NE(t4, t5);
}

TEST_F(TestIRTypesFactory, TestStructs)
{
    IRTypes types;
    std::vector<const IRType*> memTy;
    std::vector<IRDetail> memDet;
    std::vector<std::string> memName;

    memTy.push_back(types.GetFloatTy());
    memDet.push_back(kIRUniform);
    memName.push_back("x");

    const IRStructType* t1 = types.GetStructType("t1", memTy, memDet, memName);
    const IRStructType* t2 = types.GetStructType("t2", memTy, memDet, memName);
    const IRStructType* t3 = types.GetStructType("t1", memTy, memDet, memName);
    EXPECT_NE(t1, t2);
    EXPECT_EQ(t1, t3);
    EXPECT_NE(t1, memTy[0]);

    memTy.push_back(types.GetArrayType(types.GetFloatTy(), 3));
    memDet.push_back(kIRVarying);
    memName.push_back("y");
                    
    const IRStructType* t4 = types.GetStructType("t1", memTy, memDet, memName);
    const IRStructType* t5 = types.GetStructType("t2", memTy, memDet, memName);
    EXPECT_NE(t4, t1);
    EXPECT_NE(t5, t2);
    EXPECT_NE(t4, t5);
}


int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
