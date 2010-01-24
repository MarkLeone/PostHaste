#include "ir/IRStmts.h"
#include "ir/IRValues.h"
#include "ir/IRTypes.h"
#include "util/UtCast.h"
#include <gtest/gtest.h>
#include <iostream>

class TestIRStmt : public testing::Test { };

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

TEST_F(TestIRStmt, TestBlock)
{
    IRTypes types;
    IRBlock* b = MakeBlock(&types);
    std::cout << *b;

    IRInsts insts = b->GetInsts();
    ASSERT_EQ(1U, insts.size());
    IRInsts::const_iterator it = insts.begin();
    IRInst* i1 = *it++;
    EXPECT_EQ(kIRBasicInst, i1->GetKind());
    EXPECT_EQ(types.GetFloatTy(), i1->GetType());
    EXPECT_EQ(kIRUniform, i1->GetDetail());

    // Test casting
    const IRStmt* s1 = UtStaticCast<const IRStmt*>(b);
    const IRStmt* s2 = UtCast<const IRStmt*>(b);
    EXPECT_TRUE(s2 != NULL);

    const IRStmt* s3 = UtStaticCast<const IRBlock*>(s1);
    const IRStmt* s4 = UtCast<const IRBlock*>(s2);
    EXPECT_TRUE(s4 != NULL);

    delete b;
}

TEST_F(TestIRStmt, TestIfStmt)
{
    IRTypes types;
    IRValue* condVar =
        new IRLocalVar("cond", types.GetBoolTy(), kIRUniform, NULL);
    IRStmt* thenStmt = MakeBlock(&types);
    IRStmt* elseStmt = new IRSeq();
    IRIfStmt ifStmt(condVar, thenStmt, elseStmt, IRPos());
    std::cout << ifStmt;

    EXPECT_EQ(condVar, ifStmt.GetCond());
    EXPECT_EQ(thenStmt, ifStmt.GetThen());
    EXPECT_EQ(elseStmt, ifStmt.GetElse());

    // Test casting
    const IRStmt* s1 = UtStaticCast<const IRStmt*>(&ifStmt);
    const IRStmt* s2 = UtCast<const IRStmt*>(&ifStmt);
    EXPECT_TRUE(s2 != NULL);

    const IRStmt* s3 = UtStaticCast<const IRIfStmt*>(s1);
    const IRStmt* s4 = UtCast<const IRIfStmt*>(s2);
    EXPECT_TRUE(s4 != NULL);
}

TEST_F(TestIRStmt, TestForStmtCondOnly)
{
    IRTypes types;
    IRValue* condVar =
        new IRLocalVar("cond", types.GetBoolTy(), kIRUniform, NULL);
    IRStmt* condStmt = new IRSeq();
    IRStmt* iterateStmt = new IRSeq();
    IRStmt* body = MakeBlock(&types);
    IRForLoop forStmt(condStmt, condVar, iterateStmt, body, IRPos());
    std::cout << forStmt;
}

TEST_F(TestIRStmt, TestForStmtWithOneCondStmt)
{
    IRTypes types;
    IRValue* condVar =
        new IRLocalVar("cond", types.GetBoolTy(), kIRUniform, NULL);
    IRStmt* condStmt = MakeBlock(&types);
    IRStmt* iterateStmt = new IRSeq();
    IRStmt* body = MakeBlock(&types);
    IRForLoop forStmt(condStmt, condVar, iterateStmt, body, IRPos());
    std::cout << forStmt;
}

TEST_F(TestIRStmt, TestForStmtWithTwoCondStmts)
{
    IRTypes types;
    IRValue* condVar =
        new IRLocalVar("cond", types.GetBoolTy(), kIRUniform, NULL);
    IRStmt* condStmt = MakeBlock(&types, true);
    IRStmt* iterateStmt = new IRSeq();
    IRStmt* body = MakeBlock(&types, true);
    IRForLoop forStmt(condStmt, condVar, iterateStmt, body, IRPos());
    std::cout << forStmt;
}

TEST_F(TestIRStmt, TestForStmtWithComplexCondStmt)
{
    IRTypes types;

    IRValue* ifVar =
        new IRLocalVar("p", types.GetBoolTy(), kIRUniform, NULL);
    IRStmt* thenStmt = MakeBlock(&types);
    IRStmt* elseStmt = new IRSeq();
    IRIfStmt* ifStmt = new IRIfStmt(ifVar, thenStmt, elseStmt, IRPos());

    IRStmts* stmts = new IRStmts;
    stmts->push_back(ifStmt);
    stmts->push_back(MakeBlock(&types, true));
    IRStmt* seq = new IRSeq(stmts);

    IRValue* loopVar =
        new IRLocalVar("cond", types.GetBoolTy(), kIRUniform, NULL);
    IRStmt* condStmt = seq;
    IRStmt* iterateStmt = new IRSeq();
    IRStmt* body = MakeBlock(&types);
    IRForLoop forStmt(condStmt, loopVar, iterateStmt, body, IRPos());
    std::cout << forStmt;
}    

TEST_F(TestIRStmt, TestForStmtWithComplexIterStmt)
{
    IRTypes types;

    IRValue* ifVar =
        new IRLocalVar("p", types.GetBoolTy(), kIRUniform, NULL);
    IRStmt* thenStmt = MakeBlock(&types);
    IRStmt* elseStmt = new IRSeq();
    IRIfStmt* ifStmt = new IRIfStmt(ifVar, thenStmt, elseStmt, IRPos());

    IRStmts* stmts = new IRStmts;
    stmts->push_back(ifStmt);
    stmts->push_back(MakeBlock(&types, true));
    IRStmt* seq = new IRSeq(stmts);

    IRValue* loopVar =
        new IRLocalVar("cond", types.GetBoolTy(), kIRUniform, NULL);
    IRStmt* condStmt = new IRSeq();
    IRStmt* iterateStmt = seq;
    IRStmt* body = MakeBlock(&types);
    IRForLoop forStmt(condStmt, loopVar, iterateStmt, body, IRPos());
    std::cout << forStmt;
}    

TEST_F(TestIRStmt, TestForStmtWithOneIterStmt)
{
    IRTypes types;
    IRValue* condVar =
        new IRLocalVar("cond", types.GetBoolTy(), kIRUniform, NULL);
    IRStmt* condStmt = new IRSeq();
    IRStmt* iterateStmt = MakeBlock(&types);
    IRStmt* body = MakeBlock(&types);
    IRForLoop forStmt(condStmt, condVar, iterateStmt, body, IRPos());
    std::cout << forStmt;
}

TEST_F(TestIRStmt, TestForStmtWithTwoIterStmts)
{
    IRTypes types;
    IRValue* condVar =
        new IRLocalVar("cond", types.GetBoolTy(), kIRUniform, NULL);
    IRStmt* condStmt = new IRSeq();
    IRStmt* iterateStmt = MakeBlock(&types, true);
    IRStmt* body = MakeBlock(&types, true);
    IRForLoop forStmt(condStmt, condVar, iterateStmt, body, IRPos());
    std::cout << forStmt;
}

TEST_F(TestIRStmt, TestSeq)
{
    IRTypes types;
    IRStmts* stmts = new IRStmts;
    IRStmt* b1 = MakeBlock(&types);
    IRStmt* b2 = new IRBlock(new IRInsts); // empty block.
    stmts->push_back(b1);
    stmts->push_back(b2);
    IRSeq seq(stmts);
    std::cout << seq;

    IRStmts got = seq.GetStmts();
    ASSERT_EQ(2U, got.size());
    IRStmts::const_iterator it = got.begin();
    EXPECT_EQ(b1, *it++);
    EXPECT_EQ(b2, *it++);

    // Test casting
    const IRStmt* s1 = UtStaticCast<const IRStmt*>(&seq);
    const IRStmt* s2 = UtCast<const IRStmt*>(&seq);
    EXPECT_TRUE(s2 != NULL);

    const IRStmt* s3 = UtStaticCast<const IRSeq*>(s1);
    const IRStmt* s4 = UtCast<const IRSeq*>(s2);
    EXPECT_TRUE(s4 != NULL);
}

TEST_F(TestIRStmt, TestIlluminanceStmt)
{
    IRTypes types;
    IRStmt* condStmt = MakeBlock(&types);
    IRValue* category = new IRStringConst("");
    IRGlobalVar pos("P", types.GetPointTy(), kIRVarying, NULL, false);
    IRValues args;
    args.push_back(&pos);
    IRStmt* body = MakeBlock(&types);
    IRIlluminanceLoop illum(category, args, body, IRPos());
    std::cout << illum;
}

TEST_F(TestIRStmt, TestIlluminateStmt)
{
    IRTypes types;
    IRStmt* condStmt = MakeBlock(&types);
    IRGlobalVar pos("P", types.GetPointTy(), kIRVarying, NULL, false);
    IRValues args;
    args.push_back(&pos);
    IRStmt* body = MakeBlock(&types);
    IRIlluminateStmt illum(false, args, body, IRPos());
    std::cout << illum;
}

TEST_F(TestIRStmt, TestGatherStmt)
{
    IRTypes types;
    IRStmt* condStmt = MakeBlock(&types);
    IRValue* category = new IRStringConst("illuminance");
    IRGlobalVar pos("P", types.GetPointTy(), kIRVarying, NULL, false);
    IRValues args;
    args.push_back(&pos);
    IRStmt* body = MakeBlock(&types, true);
    IRStmt* elseStmt = MakeBlock(&types);
    IRGatherLoop gather(category, args, body, elseStmt, IRPos());
    std::cout << gather;
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
