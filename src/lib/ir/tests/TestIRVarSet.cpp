#include "ir/IRVarSet.h"
#include "ir/IRTypes.h"
#include "ir/IRLocalVar.h"
#include <gtest/gtest.h>
#include <iostream>

class TestIRVarSet : public testing::Test { 
public:
    IRTypes mTypes;
    const IRType* mFloat;
    IRLocalVar x1, x2, x3;

    TestIRVarSet() :
        mFloat(mTypes.GetFloatTy()),
        x1("x1", mFloat, kIRUniform, ""),
        x2("x2", mFloat, kIRUniform, ""),
        x3("x3", mFloat, kIRUniform, "")
    {
    }
};

TEST_F(TestIRVarSet, TestEmpty)
{
    IRVarSet set;
    EXPECT_FALSE(set.Has(&x1));
    EXPECT_TRUE(set.IsEmpty());
    std::cout << set << std::endl;
}

TEST_F(TestIRVarSet, TestSingleton)
{
    IRVarSet set;
    set += &x1;
    EXPECT_TRUE(set.Has(&x1));
    EXPECT_FALSE(set.Has(&x2));
    EXPECT_FALSE(set.IsEmpty());
    std::cout << set << std::endl;
}

TEST_F(TestIRVarSet, TestDupe)
{
    IRVarSet set;
    set += &x1;
    set += &x1;
    EXPECT_TRUE(set.Has(&x1));
    EXPECT_FALSE(set.Has(&x2));
    std::cout << set << std::endl;
}

TEST_F(TestIRVarSet, TestDouble)
{
    IRVarSet set;
    set += &x1;
    set += &x2;
    EXPECT_TRUE(set.Has(&x1));
    EXPECT_TRUE(set.Has(&x2));
    EXPECT_FALSE(set.Has(&x3));
    std::cout << set << std::endl;
}

TEST_F(TestIRVarSet, TestRemove)
{
    IRVarSet set;
    set += &x1;
    set += &x2;
    set -= &x1;
    EXPECT_FALSE(set.Has(&x1));
    EXPECT_TRUE(set.Has(&x2));
    std::cout << set << std::endl;
    set -= &x2;
    EXPECT_FALSE(set.Has(&x1));
    EXPECT_FALSE(set.Has(&x2));
    std::cout << set << std::endl;
}

TEST_F(TestIRVarSet, TestUnion1)
{
    IRVarSet set1, set2;
    set1 += &x1;
    set2 += &x2;
    set1 += set2;
    EXPECT_TRUE(set1.Has(&x1));
    EXPECT_TRUE(set1.Has(&x2));
    std::cout << set1 << std::endl;
    std::cout << set2 << std::endl;
}

TEST_F(TestIRVarSet, TestUnion2)
{
    IRVarSet set1, set2;
    set2 += &x2;
    set1 += set2;
    EXPECT_FALSE(set1.Has(&x1));
    EXPECT_TRUE(set1.Has(&x2));
    std::cout << set1 << std::endl;
    std::cout << set2 << std::endl;
}

TEST_F(TestIRVarSet, TestUnion3)
{
    IRVarSet set1, set2;
    set1 += &x1;
    set1 += set2;
    EXPECT_TRUE(set1.Has(&x1));
    EXPECT_FALSE(set1.Has(&x2));
    std::cout << set1 << std::endl;
    std::cout << set2 << std::endl;
}


TEST_F(TestIRVarSet, TestUnion4)
{
    IRVarSet set1, set2;
    set1 += set2;
    EXPECT_FALSE(set1.Has(&x1));
    EXPECT_FALSE(set1.Has(&x2));
    std::cout << set1 << std::endl;
    std::cout << set2 << std::endl;
}

TEST_F(TestIRVarSet, TestIntersect1)
{
    IRVarSet set1, set2;
    set1 += &x1;
    set2 += &x2;
    set2 += &x1;
    set1.Intersect(set2);
    EXPECT_TRUE(set1.Has(&x1));
    std::cout << set1 << std::endl;
}

TEST_F(TestIRVarSet, TestIntersect2)
{
    IRVarSet set1, set2;
    set1 += &x1;
    set2 += &x2;
    set1.Intersect(set2);
    EXPECT_FALSE(set1.Has(&x1));
    std::cout << set1 << std::endl;
}

TEST_F(TestIRVarSet, TestIntersect3)
{
    IRVarSet set1, set2;
    set1 += &x1;
    set2 += &x1;
    set1.Intersect(set2);
    EXPECT_TRUE(set1.Has(&x1));
    std::cout << set1 << std::endl;
}

TEST_F(TestIRVarSet, TestIntersect4)
{
    IRVarSet set1, set2;
    set1 += &x1;
    set1.Intersect(set2);
    EXPECT_FALSE(set1.Has(&x1));
    std::cout << set1 << std::endl;
}

TEST_F(TestIRVarSet, TestIntersect5)
{
    IRVarSet set1, set2;
    set1 += &x1;
    set1 += &x2;
    set2 += &x3;
    set2 += &x2;
    set2 += &x1;
    set1.Intersect(set2);
    EXPECT_TRUE(set1.Has(&x1));
    EXPECT_TRUE(set1.Has(&x2));
    std::cout << set1 << std::endl;
}

TEST_F(TestIRVarSet, TestIntersect6)
{
    IRVarSet set1, set2;
    set1 += &x1;
    set1 += &x2;
    set2 += &x2;
    set2 += &x1;
    set1.Intersect(set2);
    EXPECT_TRUE(set1.Has(&x1));
    EXPECT_TRUE(set1.Has(&x2));
    std::cout << set1 << std::endl;
}

TEST_F(TestIRVarSet, TestIntersect7)
{
    IRVarSet set1, set2;
    set1 += &x1;
    set1 += &x2;
    set2 += &x1;
    set1.Intersect(set2);
    EXPECT_TRUE(set1.Has(&x1));
    EXPECT_FALSE(set1.Has(&x2));
    std::cout << set1 << std::endl;
}

TEST_F(TestIRVarSet, TestIntersect8)
{
    IRVarSet set1, set2;
    set1 += &x1;
    set1 += &x2;
    set1.Intersect(set2);
    EXPECT_FALSE(set1.Has(&x1));
    EXPECT_FALSE(set1.Has(&x2));
    std::cout << set1 << std::endl;
}

TEST_F(TestIRVarSet, TestIntersect9)
{
    IRVarSet set1, set2;
    set2 += &x2;
    set1.Intersect(set2);
    EXPECT_TRUE(set1.IsEmpty());
    std::cout << set1 << std::endl;
}

TEST_F(TestIRVarSet, TestIntersect10)
{
    IRVarSet set1, set2;
    set1.Intersect(set2);
    EXPECT_TRUE(set1.IsEmpty());
    std::cout << set1 << std::endl;
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

