#include "ops/OpVec3.h"
#include <gtest/gtest.h>
#include <iostream>
#include <stdio.h>

class TestOpVec3 : public testing::Test { };

TEST_F(TestOpVec3, TestConstruct) {
    OpVec3 v1(0.0f);
    EXPECT_FLOAT_EQ(0.0f, v1[0]);

    OpVec3 v2(1,2,3);
    EXPECT_FLOAT_EQ(3.0f, v2[2]);

    OpVec3 v3(v2);
    EXPECT_FLOAT_EQ(3.0f, v3[2]);

    OpVec3 v4;
    v4 = v3;
    EXPECT_FLOAT_EQ(3.0f, v4[2]);
}

TEST_F(TestOpVec3, TestIndex) {
    OpVec3 v1(1);
    v1[(int) v1[0]] = 2;
    EXPECT_FLOAT_EQ(2.0f, v1[1]);
    EXPECT_FLOAT_EQ(1.0f, v1[0]);
}

TEST_F(TestOpVec3, TestEq) {
    OpVec3 v1(1,2,3);
    OpVec3 v2(2,3,4);
    EXPECT_TRUE(v1 == v1);
    EXPECT_FALSE(v1 == v2);
    EXPECT_TRUE(v1 != v2);
    EXPECT_FALSE(v1 != v1);
}

TEST_F(TestOpVec3, TestDot) {
    OpVec3 v1(1,2,3);
    OpVec3 v2(2,3,4);
    EXPECT_FLOAT_EQ(20.0f, v1 * v2);
}

TEST_F(TestOpVec3, TestSum) {
    OpVec3 v1(1,2,3);
    OpVec3 v2(2,3,4);
    EXPECT_TRUE(v1 + v2 == OpVec3(3,5,7));
}

TEST_F(TestOpVec3, TestDifference) {
    OpVec3 v1(2,3,4);
    OpVec3 v2(1,2,3);
    EXPECT_TRUE(v1 - v2 == OpVec3(1));
}

TEST_F(TestOpVec3, TestNegate) {
    EXPECT_TRUE(-OpVec3(1,2,3) == OpVec3(-1,-2,-3));
}

TEST_F(TestOpVec3, TestScale) {
    OpVec3 v1 = OpVec3(1,2,3) * 2;
    EXPECT_FLOAT_EQ(2, v1[0]);
    EXPECT_FLOAT_EQ(4, v1[1]);
    EXPECT_FLOAT_EQ(6, v1[2]);
    OpVec3 v2 = 2 * OpVec3(1,2,3);
    EXPECT_TRUE(v1 == v2);
}

TEST_F(TestOpVec3, TestDivide) {
    EXPECT_TRUE(OpVec3(2,4,6) / 2 == OpVec3(1,2,3));
}

TEST_F(TestOpVec3, TestLength) {
    EXPECT_FLOAT_EQ(5, OpVec3(3,4,0).Length());
    EXPECT_FLOAT_EQ(sqrtf(29.0f), OpVec3(2,3,4).Length());
}

TEST_F(TestOpVec3, TestNormalized) {
    OpVec3 n = OpVec3(3,4,0).Normalized();
    EXPECT_FLOAT_EQ(.6f, n[0]);
    EXPECT_FLOAT_EQ(.8f, n[1]);

    EXPECT_FLOAT_EQ(1.0f, OpVec3(1,2,3).Normalized().Length());
    EXPECT_FLOAT_EQ(1.0f, OpVec3(-1,2,0).Normalized().Length());
    EXPECT_FLOAT_EQ(1.0f, OpVec3(0,0,-3).Normalized().Length());

    OpVec3 x(1,0,0);
    OpVec3 y(0,1,0);
    OpVec3 z(0,0,1);
    EXPECT_TRUE(x.Normalized() == x);
    EXPECT_TRUE(y.Normalized() == y);
    EXPECT_TRUE(z.Normalized() == z);
}

float Inc(float x) { return x+1; }
float Sum(float x1, float x2) { return x1+x2; }

TEST_F(TestOpVec3, TestMap) {
    OpVec3 v1(1,2,3);
    OpVec3 v2(2,3,4);
    OpVec3 v3(3,5,7);
    EXPECT_TRUE(OpMap<Inc>(v1) == v2);
    EXPECT_TRUE(OpMap2<Sum>(v1, v2) == v3);
}

TEST_F(TestOpVec3, TestOutput) {
    std::cout << OpVec3(1,2,3) << std::endl;
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
