#include "ops/OpMatrix4.h"
#include <gtest/gtest.h>
#include <iostream>
#include <stdio.h>

class TestOpMatrix4 : public testing::Test { };

TEST_F(TestOpMatrix4, TestConstruct) {
    OpMatrix4 m1(1);
    std::cout << m1 << std::endl;
    EXPECT_FLOAT_EQ(1.0f, m1[0][0]);
    EXPECT_FLOAT_EQ(1.0f, m1[1][1]);
    EXPECT_FLOAT_EQ(0.0f, m1[0][1]);
    EXPECT_FLOAT_EQ(0.0f, m1[1][0]);

    OpMatrix4 m2(OpVec4(1), OpVec4(2), OpVec4(3), OpVec4(4));
    std::cout << m2 << std::endl;
    EXPECT_FLOAT_EQ(1.0f, m2[0][1]);
    EXPECT_FLOAT_EQ(2.0f, m2[1][0]);

    OpMatrix4 m3(m2);
    std::cout << m3 << std::endl;

    OpMatrix4 m4;
    m4 = m3;
    std::cout << m4 << std::endl;

    OpMatrix4 m5(1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16);
    std::cout << m5 << std::endl;
}

TEST_F(TestOpMatrix4, TestIndexing) {
    OpMatrix4 m1(1);
    EXPECT_TRUE(m1[0] == OpVec4(1,0,0,0));
    EXPECT_TRUE(m1[3] == OpVec4(0,0,0,1));

    m1[0] = OpVec4(1,1,1,1);
    m1[3][1] = 4;
    std::cout << m1 << std::endl;
}

TEST_F(TestOpMatrix4, TestTranspose) {
    OpMatrix4 m1(1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16);
    OpMatrix4 m2 = m1.Transposed();
    std::cout << m2 << std::endl;
    EXPECT_FLOAT_EQ(4.0f, m2[3][0]);
    EXPECT_FLOAT_EQ(13.0f, m2[0][3]);
}

TEST_F(TestOpMatrix4, TestMultiply) {
    OpMatrix4 id(1.0f);
    OpMatrix4 m1(1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16);

    OpMatrix4 m2 = m1.Multiply(id);
    std::cout << m2 << std::endl;
    EXPECT_TRUE(m2 == m1);

    OpMatrix4 m3 = id.Multiply(m1);
    EXPECT_TRUE(m3 == m1);
    
    OpMatrix4 m4(.1,.1,.1,.1, .2,.2,.2,.2, -1,-1,-1,-1, -2,-2,-2,-2);
    OpMatrix4 m5 = m4.Multiply(m1);
    std::cout << m5 << std::endl;
    EXPECT_FLOAT_EQ(2.8f, m5[0][0]);
    EXPECT_FLOAT_EQ(4.0f, m5[0][3]);
    EXPECT_FLOAT_EQ(-28.0f, m5[2][0]);
    EXPECT_FLOAT_EQ(-40.0f, m5[2][3]);
}

TEST_F(TestOpMatrix4, TestInverse) {
    OpMatrix4 id(1.0f);
    OpMatrix4 inv = id.Inverse();
    std::cout << inv << std::endl;
    EXPECT_TRUE(inv == id);
}

TEST_F(TestOpMatrix4, TestDeterminant) {
    float d = OpMatrix4::Det3x3(0, 1, 2,
                                3, 4, 5,
                                6, 7, 8);
    EXPECT_FLOAT_EQ(0.0f, d);

    OpMatrix4 id(1.0f);
    EXPECT_FLOAT_EQ(1.0f, id.Determinant());

    // The determinant of a matrix with two identical rows is zero.
    OpMatrix4 m1(1,2,3,4,
                 1,2,3,4,
                 5,6,7,8,
                 5,6,7,8);
    EXPECT_FLOAT_EQ(0.0f, m1.Determinant());
}


TEST_F(TestOpMatrix4, TestCast) {
    OpMatrix4 m(1);
    typedef float RtMatrix[4][4];
    RtMatrix& mx = *reinterpret_cast<RtMatrix*>(&m);
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
