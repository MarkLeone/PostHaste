#include "ops/OpVec4.h"
#include <gtest/gtest.h>
#include <iostream>
#include <stdio.h>

class TestOpVec4 : public testing::Test { };

TEST_F(TestOpVec4, TestConstruct) {
    OpVec4 v1(0);
    EXPECT_FLOAT_EQ(0.0f, v1[0]);

    OpVec4 v2(1,2,3,4);
    EXPECT_FLOAT_EQ(3.0f, v2[2]);

    OpVec4 v3(v2);
    EXPECT_FLOAT_EQ(3.0f, v3[2]);

    OpVec4 v4;
    v4 = v3;
    EXPECT_FLOAT_EQ(3.0f, v4[2]);
}

TEST_F(TestOpVec4, TestIndex) {
    OpVec4 v1(1);
    v1[(int) v1[0]] = 2;
    EXPECT_FLOAT_EQ(2.0f, v1[1]);
    EXPECT_FLOAT_EQ(1.0f, v1[0]);
}

TEST_F(TestOpVec4, TestEq) {
    OpVec4 v1(1,2,3,4);
    OpVec4 v2(2,3,4,5);
    EXPECT_TRUE(v1 == v1);
    EXPECT_FALSE(v1 == v2);
    EXPECT_TRUE(v1 != v2);
    EXPECT_FALSE(v1 != v1);
}

TEST_F(TestOpVec4, TestDot) {
    OpVec4 v1(1,2,3,4);
    OpVec4 v2(2,3,4,5);
    EXPECT_FLOAT_EQ(40.0f, v1 * v2);
}

TEST_F(TestOpVec4, TestSum) {
    OpVec4 v1(1,2,3,4);
    OpVec4 v2(2,3,4,5);
    EXPECT_TRUE(v1 + v2 == OpVec4(3,5,7,9));
}

TEST_F(TestOpVec4, TestDifference) {
    OpVec4 v1(2,3,4,5);
    OpVec4 v2(1,2,3,4);
    EXPECT_TRUE(v1 - v2 == OpVec4(1));
}

TEST_F(TestOpVec4, TestNegate) {
    EXPECT_TRUE(-OpVec4(1,2,3,4) == OpVec4(-1,-2,-3,-4));
}

TEST_F(TestOpVec4, TestScale) {
    OpVec4 v1 = OpVec4(1,2,3,4) * 2;
    EXPECT_FLOAT_EQ(2, v1[0]);
    EXPECT_FLOAT_EQ(4, v1[1]);
    EXPECT_FLOAT_EQ(6, v1[2]);
    EXPECT_FLOAT_EQ(8, v1[3]);
    OpVec4 v2 = 2 * OpVec4(1,2,3,4);
    EXPECT_TRUE(v1 == v2);
}

TEST_F(TestOpVec4, TestDivide) {
    EXPECT_TRUE(OpVec4(2,4,6,8) / 2 == OpVec4(1,2,3,4));
}

TEST_F(TestOpVec4, TestOutput) {
    std::cout << OpVec4(1,2,3,4) << std::endl;
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
