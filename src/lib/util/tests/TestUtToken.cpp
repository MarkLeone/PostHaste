#include "util/UtToken.h"
#include "util/UtTokenFactory.h"
#include <gtest/gtest.h>
#include <list>
#include <stdio.h>
#include <vector>

class TestUtToken : public testing::Test { };

TEST_F(TestUtToken, TestTokens)
{
    UtTokenFactory factory;
    UtToken t1 = factory.Get("foo");
    UtToken t2 = factory.Get("foo");
    UtToken t3 = factory.Get("bar");
    EXPECT_TRUE(t1 == t2);
    EXPECT_EQ(t1, t2);
    EXPECT_TRUE(t1 != t3);
    EXPECT_NE(t1, t3);

    char* s1 = strdup("foo");
    char* s2 = strdup("bar");
    UtToken t4 = factory.Get(s1);
    UtToken t5 = factory.Get(s2);
    EXPECT_NE(t4, t5);
    EXPECT_EQ(t1, t4);
    EXPECT_EQ(t2, t4);
    EXPECT_EQ(t3, t5);

    std::string str1 = s1;
    std::string str2 = s2;
    UtToken t6 = factory.Get(str1);
    UtToken t7 = factory.Get(str2);
    EXPECT_NE(t6, t7);
    EXPECT_TRUE(t1 == t6);
    EXPECT_EQ(t2, t6);
    EXPECT_EQ(t3, t7);

    // Test explicit/implict conversion.
    std::cout << static_cast<const char*>(t1) << std::endl;
    std::cout << std::string(t1) << std::endl;
    std::cout << t1 << std::endl;
    const char* name1 = t1;
    std::cout << name1 << std::endl;

    free(s1);
    free(s2);
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
