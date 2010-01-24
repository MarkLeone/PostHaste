#include "util/UtDelete.h"
#include <gtest/gtest.h>
#include <list>
#include <stdio.h>
#include <vector>

class TestUtDelete : public testing::Test { };

TEST_F(TestUtDelete, TestDeleteList)
{
    std::list<int*>* list = new std::list<int*>;
    list->push_back(new int);
    list->push_back(new int);
    UtDeleteSeq(list);
}

TEST_F(TestUtDelete, TestDeleteListRef)
{
    std::list<int*> list;
    list.push_back(new int);
    list.push_back(new int);
    UtDeleteSeq(list);
}

TEST_F(TestUtDelete, TestDeleteVec)
{
    std::vector<int*>* vec = new std::vector<int*>;
    vec->push_back(new int);
    vec->push_back(new int);
    UtDeleteSeq(vec);
}

TEST_F(TestUtDelete, TestDeleteVecRef)
{
    std::vector<int*> vec;
    vec.push_back(new int);
    vec.push_back(new int);
    UtDeleteSeq(vec);
}

struct Element {
    int x;
    Element(int y) : x(y) { }
    ~Element() { printf("~Element: %i\n", x); }
};

TEST_F(TestUtDelete, TestDeleteElement)
{
    std::vector<Element*> vec;
    vec.push_back(new Element(1));
    vec.push_back(new Element(2));
    UtDeleteSeq(vec);
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
