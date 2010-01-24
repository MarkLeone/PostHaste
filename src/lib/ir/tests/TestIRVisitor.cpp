#include "ir/IRTypes.h"
#include "ir/IRValues.h"
#include "ir/IRVisitor.h"
#include <gtest/gtest.h>
#include <iostream>

class TestIRVisitor : public testing::Test { };

class Walker : public IRVisitor<Walker> {
public:
    void Visit(IRBlock* stmt, int dummy) {
        std::cout << "IRBlock\n"; 
        const IRInsts& insts = stmt->GetInsts();
        IRInsts::const_iterator it;
        for (it = insts.begin(); it != insts.end(); ++it)
            Dispatch<void>(*it, dummy);
    }
    void Visit(IRIfStmt* stmt, int dummy) { 
        std::cout << "IRIfStmt\n"; 
        Dispatch<void>(stmt->GetThen(), dummy);
        Dispatch<void>(stmt->GetElse(), dummy);
    }
    void Visit(IRSeq* stmt, int dummy) {
        std::cout << "IRSeq\n"; 
        const IRStmts& stmts = stmt->GetStmts();
        IRStmts::const_iterator it;
        for (it = stmts.begin(); it != stmts.end(); ++it)
            Dispatch<void>(*it, dummy);
    }

    // Do nothing for gather, illuminance, illuminate.
    void Visit(IRStmt* stmt, int dummy) { }

    void Visit(IRInst* inst, int dummy) { 
        std::cout << inst->GetName() << "\n"; 
    }
};

static IRBlock*
MakeBlock(IRTypes* types, bool twoInsts = false)
{
    // XXX this variable leaks.  Needs to be owned by shader.
    IRVar* var = new IRLocalVar("x", types->GetFloatTy(), kIRUniform, ""); 
    IRValues args;
    args.push_back(var);
    IRInsts* insts = new IRInsts;
    insts->push_back(new IRInst(kIRBasicInst, kOpcode_Negate, var, args));
    if (twoInsts)
        insts->push_back(new IRInst(kIRBasicInst, kOpcode_Sin, var, args));
    return new IRBlock(insts);
}

TEST_F(TestIRVisitor, TestWalker)
{
    IRTypes types;
    IRValue* condVar =
        new IRLocalVar("cond", types.GetBoolTy(), kIRUniform, NULL);
    IRStmt* thenStmt = MakeBlock(&types);
    IRStmt* elseStmt = MakeBlock(&types, true);
    IRIfStmt ifStmt(condVar, thenStmt, elseStmt, IRPos());
    Walker().Dispatch<void>(&ifStmt, 0);
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
