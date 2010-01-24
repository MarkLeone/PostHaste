#include "ir/IRInst.h"
#include "ir/IRStmt.h"
#include <gtest/gtest.h>
#include <iostream>

class TestIRPos : public testing::Test { };

TEST_F(TestIRPos, TestEmptyPos)
{
    IRPos pos;
    std::cout << pos << std::endl;
}

TEST_F(TestIRPos, TestFileLine)
{
    IRStringConst file("filename");
    IRPos pos(&file, 7);
    std::cout << pos << std::endl;
}

TEST_F(TestIRPos, TestFileOnly)
{
    IRStringConst file("filename");
    IRPos pos(&file, 0);
    std::cout << pos << std::endl;
}

TEST_F(TestIRPos, TestLineOnly)
{
    IRPos pos(NULL, 7);
    std::cout << pos << std::endl;
}

TEST_F(TestIRPos, TestEmptyFile)
{
    IRStringConst file("");
    IRPos pos(&file, 7);
    std::cout << pos << std::endl;
}


int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

