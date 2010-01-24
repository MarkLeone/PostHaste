#include "ir/IRValues.h"
#include "ir/IRShader.h"
#include "ir/IRStmts.h"
#include "ir/IRTypes.h"
#include <gtest/gtest.h>

class TestIRShader : public testing::Test { };

TEST_F(TestIRShader, TestShader)
{
    IRShaderParams* params = new IRShaderParams;
    IRConsts* constants = new IRConsts;
    IRLocalVars* locals = new IRLocalVars;
    IRGlobalVars* globals = new IRGlobalVars;
    IRTypes* types = new IRTypes;
    
    params->push_back(new IRShaderParam("param", types->GetFloatTy(),
                                        kIRUniform, "", false, new IRSeq()));
    constants->push_back(new IRNumConst(1.0f));
    locals->push_back(new IRLocalVar("local", types->GetFloatTy(),                                     kIRVarying, ""));
    globals->push_back(new IRGlobalVar("global", types->GetPointTy(),
                                       kIRVarying, "", false));

    // Make the body.
    IRLocalVar var("x", types->GetFloatTy(), kIRUniform, ""); 
    IRValues args;
    args.push_back(&var);
    IRInst* i1 = new IRInst(kIRBasicInst, kOpcode_Negate, &var, args);
    IRInsts* insts = new IRInsts;
    insts->push_back(i1);
    IRStmt* body = new IRBlock(insts);

    IRShader shader("foo", kSloSurface, types, params, constants, 
                    locals, globals, body);
    std::cout << shader;

    EXPECT_STREQ("foo", shader.GetName());
    EXPECT_EQ(kSloSurface, shader.GetType());
    EXPECT_EQ(params, &shader.GetParams());
    EXPECT_EQ(body, shader.GetBody());
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

