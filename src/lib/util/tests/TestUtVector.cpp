#include "util/UtVector.h"
#include <gtest/gtest.h>
#include <iostream>

class TestUtVector : public testing::Test { };

TEST_F(TestUtVector, TestIndexing)
{
    UtVector<int> nums;
    nums.push_back(1);
    nums.push_back(2);
    nums.push_back(3);
    std::cout << nums << std::endl;
    
    int i = 0;
    unsigned int j = 1;
    size_t k = 2;
    EXPECT_EQ(1, nums[i]);
    EXPECT_EQ(2, nums[j]);
    EXPECT_EQ(3, nums[k]);
}

TEST_F(TestUtVector, TestDupCtor)
{
    UtVector<float> nums(4, 1.0f);
    std::cout << nums << std::endl;
    EXPECT_EQ(1.0f, nums[0]);
    EXPECT_EQ(1.0f, nums[3]);
}

TEST_F(TestUtVector, TestRangeCtor)
{
    int nums[] = {1,2,3,4};
    UtVector<int> v1(nums, nums+4);
    UtVector<int> v2(nums+2, nums+4);
    std::cout << v1 << std::endl;
    std::cout << v2 << std::endl;
    EXPECT_EQ(4U, v1.size());
    EXPECT_EQ(2U, v2.size());
}

TEST_F(TestUtVector, TestCopyCtor)
{
    int nums[] = {1,2,3,4};
    UtVector<int> v1(nums, nums+4);
    UtVector<int> v2(v1);
    std::cout << v2 << std::endl;
    EXPECT_EQ(4U, v2.size());
    EXPECT_EQ(4, v2[3]);
}

TEST_F(TestUtVector, TestVarArgsCtor)
{
    UtVector<int> v0(1, 1);
    std::cout << v0 << std::endl;
    UtVector<int> v1(3, 1,2,3);
    std::cout << v1 << std::endl;
    float f1 = 1.1, f2 = 2.2;
    UtVector<float*> v2(2, &f1, &f2);
    std::cout << *v2[0] << ' ' << *v2[1] << std::endl;
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
