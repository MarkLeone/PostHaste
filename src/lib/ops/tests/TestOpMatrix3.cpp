#include "ops/OpMatrix3.h"
#include <gtest/gtest.h>
#include <iostream>
#include <stdio.h>

class TestOpMatrix3 : public testing::Test { };

TEST_F(TestOpMatrix3, TestConstruct) {
    OpMatrix3 m1(1);
    std::cout << m1 << std::endl;
    EXPECT_FLOAT_EQ(1.0f, m1[0][0]);
    EXPECT_FLOAT_EQ(1.0f, m1[1][1]);
    EXPECT_FLOAT_EQ(0.0f, m1[0][1]);
    EXPECT_FLOAT_EQ(0.0f, m1[1][0]);

    OpMatrix3 m2(OpVec3(1), OpVec3(2), OpVec3(3));
    std::cout << m2 << std::endl;
    EXPECT_FLOAT_EQ(1.0f, m2[0][1]);
    EXPECT_FLOAT_EQ(2.0f, m2[1][0]);

    OpMatrix3 m3(m2);
    std::cout << m3 << std::endl;

    OpMatrix3 m4;
    m4 = m3;
    std::cout << m4 << std::endl;

    OpMatrix3 m5(1,2,3, 4,5,6, 7,8,9);
    std::cout << m5 << std::endl;
}

TEST_F(TestOpMatrix3, TestIndexing) {
    OpMatrix3 m1(1);
    EXPECT_TRUE(m1[0] == OpVec3(1,0,0));
    EXPECT_TRUE(m1[2] == OpVec3(0,0,1));

    m1[0] = OpVec3(1,1,1);
    m1[2][1] = 3;
    std::cout << m1 << std::endl;
}

TEST_F(TestOpMatrix3, TestTranspose) {
    OpMatrix3 m1(1,2,3, 4,5,6, 7,8,9);
    OpMatrix3 m2 = m1.Transposed();
    std::cout << m2 << std::endl;
    EXPECT_FLOAT_EQ(3.0f, m2[2][0]);
    EXPECT_FLOAT_EQ(7.0f, m2[0][2]);
}

TEST_F(TestOpMatrix3, TestMultiply) {
    OpMatrix3 id(1.0f);
    OpMatrix3 m1(1,2,3, 4,5,6, 7,8,9);

    OpMatrix3 m2 = m1.Multiply(id);
    std::cout << m2 << std::endl;
    EXPECT_TRUE(m2 == m1);

    OpMatrix3 m3 = id.Multiply(m1);
    EXPECT_TRUE(m3 == m1);
    
    OpMatrix3 m4(.1,.1,.1, .2,.2,.2, -1,-1,-1);
    OpMatrix3 m5 = m4.Multiply(m1);
    std::cout << m5 << std::endl;
    EXPECT_FLOAT_EQ(1.2f, m5[0][0]);
    EXPECT_FLOAT_EQ(1.8f, m5[0][2]);
    EXPECT_FLOAT_EQ(-12.0f, m5[2][0]);
    EXPECT_FLOAT_EQ(-18.0f, m5[2][2]);
}

TEST_F(TestOpMatrix3, TestDeterminant) {
    OpMatrix3 m1(0, 1, 2,
                 3, 4, 5,
                 6, 7, 8);
    EXPECT_FLOAT_EQ(0.0f, m1.Determinant());

    OpMatrix3 id(1.0f);
    EXPECT_FLOAT_EQ(1.0f, id.Determinant());

    // The determinant of a matrix with two identical rows is zero.
    OpMatrix3 m2(1,2,3,
                 1,2,3,
                 4,5,6);
    EXPECT_FLOAT_EQ(0.0f, m2.Determinant());
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
