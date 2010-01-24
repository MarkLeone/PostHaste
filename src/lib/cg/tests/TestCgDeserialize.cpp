#include "cg/CgDeserialize.h"
#include <gtest/gtest.h>
#include <iostream>
#include <llvm/Module.h>

class TestCgDeserialize : public testing::Test { };

TEST_F(TestCgDeserialize, TestDeserializeShadeops)
{
    llvm::Module* module = CgDeserializeShadeops(&llvm::getGlobalContext());
    llvm::Function* add = module->getFunction("OpAdd_ff");
    EXPECT_TRUE(add != NULL);
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
