#include "util/UtHashMap.h"
#include <gtest/gtest.h>

class TestUtHashMap : public testing::Test { };

TEST_F(TestUtHashMap, TestCStringHash)
{
    UtHash hashFn;
    const char* s1 = "hello";
    char* s2 = (char*) strdup(s1);
    EXPECT_NE(s1, s2);
    EXPECT_EQ(hashFn(s1), hashFn(s2));
    free(s2);
}

TEST_F(TestUtHashMap, TestCStringEq)
{
    UtHashEqual eq;
    const char* s1 = "hello";
    char* s2 = (char*) strdup(s1);
    EXPECT_NE(s1, s2);
    EXPECT_TRUE(eq(s1, s2));
    EXPECT_FALSE(eq(s1, ""));
    EXPECT_FALSE(eq(s1, "foo"));
    free(s2);
}

TEST_F(TestUtHashMap, TestStdStringHash)
{
    UtHash hashFn;
    const char* s1 = "hello";
    char* s2 = strdup(s1);
    std::string str1 = s1;
    std::string str2 = s2;
    EXPECT_NE(str1.c_str(), str2.c_str());
    EXPECT_EQ(hashFn(str1), hashFn(str2));
    free(s2);
}

TEST_F(TestUtHashMap, TestStdStringEq)
{
    UtHashEqual eq;
    const char* s1 = "hello";
    char* s2 = (char*) strdup(s1);
    std::string str1 = s1;
    std::string str2 = s2;

    EXPECT_NE(str1.c_str(), str2.c_str());
    EXPECT_TRUE(eq(str1, str2));
    EXPECT_FALSE(eq(str1, ""));
    EXPECT_FALSE(eq(str1, "foo"));
    free(s2);
}

// Need a global to defeat common subexpression elimination in TestPtrHash
// and TestPtrEq.
int* gPtr;

TEST_F(TestUtHashMap, TestPtrHash)
{
    UtHash hashFn;
    int* ptr = new int;
    gPtr = ptr;
    EXPECT_EQ(ptr, gPtr);
    EXPECT_EQ(hashFn(ptr), hashFn(gPtr));
    delete ptr;
}

TEST_F(TestUtHashMap, TestPtrEq)
{
    UtHashEqual eq;
    int* ptr1 = new int;
    int* ptr2 = new int;
    gPtr = ptr1;
    EXPECT_EQ(ptr1, gPtr);
    EXPECT_TRUE(eq(ptr1, gPtr));
    EXPECT_FALSE(eq(ptr1, static_cast<int*>(NULL)));
    delete ptr1;
    delete ptr2;
}

TEST_F(TestUtHashMap, TestConstCStringMap)
{
    UtHashMap<const char*, int> map;
    map["a"] = 1;
    map["b"] = 2;
    EXPECT_TRUE(map.find("a") != map.end());
    EXPECT_EQ(1, map["a"]);

    char* key = strdup("a");
    EXPECT_TRUE(map.find(key) != map.end());
    EXPECT_EQ(1, map[key]);

    map[key] = 3;
    EXPECT_EQ(3, map["a"]);

    free(key);
}

TEST_F(TestUtHashMap, TestCStringMap)
{
    UtHashMap<char*, int> map;
    char* a = strdup("a");
    char* b = strdup("b");
    map[a] = 1;
    map[b] = 2;
    EXPECT_TRUE(map.find(a) != map.end());
    EXPECT_EQ(1, map[a]);

    char* key = strdup(a);
    EXPECT_TRUE(map.find(key) != map.end());
    EXPECT_EQ(1, map[key]);

    map[key] = 3;
    EXPECT_EQ(3, map[a]);

    free(a);
    free(b);
    free(key);
}

TEST_F(TestUtHashMap, TestStdStringMap)
{
    UtHashMap<std::string, int> map;
    map["a"] = 1;
    map["b"] = 2;
    EXPECT_TRUE(map.find("a") != map.end());
    EXPECT_EQ(1, map["a"]);

    char* keyVal = strdup("a");
    std::string key = keyVal;
    free(keyVal);
    EXPECT_TRUE(map.find(key) != map.end());
    EXPECT_EQ(1, map[key]);

    map[key] = 3;
    EXPECT_EQ(3, map["a"]);
}


// Need a global to defeat common subexpression elimination in TestPtrMap.
int* gKey;

TEST_F(TestUtHashMap, TestPtrMap)
{
    UtHashMap<int*, int> map;
    int* p1 = new int;
    int* p2 = new int;

    map[p1] = 1;
    map[p2] = 2;
    EXPECT_TRUE(map.find(p1) != map.end());
    EXPECT_EQ(1, map[p1]);

    gKey = p1;
    EXPECT_TRUE(map.find(gKey) != map.end());
    EXPECT_EQ(1, map[gKey]);

    map[gKey] = 3;
    EXPECT_EQ(3, map[p1]);

    delete p1;
    delete p2;
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
