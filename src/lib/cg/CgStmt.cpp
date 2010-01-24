// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "cg/CgStmt.h"
#include "cg/CgInst.h"
#include "cg/CgValue.h"
#include "ir/IRInst.h"
#include "ops/Opcode.h"
#include "util/UtLog.h"
#include <llvm/DerivedTypes.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Type.h>
#include <llvm/Value.h>

void 
CgStmt::Codegen(IRStmt* stmt) const
{
    const_cast<CgStmt*>(this)->Dispatch<void>(stmt, 0);
}

void 
CgStmt::Visit(IRBlock* block, int ignored)
{
    const IRInsts& insts = block->GetInsts();
    IRInsts::const_iterator it;
    for (it = insts.begin(); it != insts.end(); ++it) {
        const IRInst* inst = *it;
        bool ok = mInsts->GenInst(*inst);
        if (!ok)
            mLog->Write(kUtError, "Codegen unimplemented for instruction '%s'",
                        inst->GetName());
    }
}

void 
CgStmt::Visit(IRSeq* seq, int ignored)
{
    const IRStmts& stmts = seq->GetStmts();
    IRStmts::const_iterator it;
    for (it = stmts.begin(); it != stmts.end(); ++it)
        Codegen(*it);
}

void 
CgStmt::Visit(IRIfStmt* ifStmt, int ignored)
{
    // Convert the condition to an LLVM value, and then convert the resulting
    // bool (i32) to a bit for LLVM's conditional branch instruction.
    llvm::Value* condVal = mValues->ConvertArg(ifStmt->GetCond());
    condVal = mValues->BoolToBit(condVal);

    // Construct basic blocks for "then" and "else" branches, along with a
    // "merge" block for the code following the "if" statement.
    llvm::Function* function = mBuilder->GetInsertBlock()->getParent();
    llvm::BasicBlock *thenBlock =
        llvm::BasicBlock::Create(*mContext, "then", function);
    llvm::BasicBlock *elseBlock =
        llvm::BasicBlock::Create(*mContext, "else");
    llvm::BasicBlock *mergeBlock =
        llvm::BasicBlock::Create(*mContext, "ifcont");

    // Generate conditional branch instruction.
    mBuilder->CreateCondBr(condVal, thenBlock, elseBlock);

    // Generate code for 'then' branch.
    mBuilder->SetInsertPoint(thenBlock);
    Codegen(ifStmt->GetThen());
    mBuilder->CreateBr(mergeBlock);

    // Add the else block to the function and generate code for 'else' branch.
    function->getBasicBlockList().push_back(elseBlock);
    mBuilder->SetInsertPoint(elseBlock);
    Codegen(ifStmt->GetElse());
    mBuilder->CreateBr(mergeBlock);

    // Add the merge block to the function and set the builder insertion point
    // before returning.
    function->getBasicBlockList().push_back(mergeBlock);
    mBuilder->SetInsertPoint(mergeBlock);
}

void 
CgStmt::Visit(IRForLoop* loop, int ignored)
{
    // Create basic blocks.
    llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(*mContext, "cond");
    llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(*mContext, "body");
    llvm::BasicBlock* exitBlock = llvm::BasicBlock::Create(*mContext, "exit");
    llvm::BasicBlock* iterBlock = llvm::BasicBlock::Create(*mContext, "iter");

    // Keep track of the exit block and iterate block, which might be targeted
    // by break/continue statements.
    loop->mBreakTarget = exitBlock;
    loop->mContinueTarget = iterBlock;

    // Add condition block and generate a jump to it.
    llvm::Function* function = mBuilder->GetInsertBlock()->getParent();
    function->getBasicBlockList().push_back(condBlock);
    mBuilder->CreateBr(condBlock);

    // Generate code for the loop condition.
    mBuilder->SetInsertPoint(condBlock);
    Codegen(loop->GetCondStmt());

    // Convert the condition to an LLVM value, and then convert the resulting
    // bool (i32) to a bit for LLVM's conditional branch instruction.
    llvm::Value* condVal = mValues->ConvertArg(loop->GetCond());
    condVal = mValues->BoolToBit(condVal);

    // Generate conditional branch to the loop body vs. the loop exit.
    mBuilder->CreateCondBr(condVal, bodyBlock, exitBlock);

    // Generate code for the loop body.
    function->getBasicBlockList().push_back(bodyBlock);
    mBuilder->SetInsertPoint(bodyBlock);

    // Generate code for the loop body.
    Codegen(loop->GetBody());

    // Generate a jump to the 'iterate' block and generate the loop iteration
    // code, followed by a jump to the conditional test at the start of the
    // loop.
    mBuilder->CreateBr(iterBlock); 
    function->getBasicBlockList().push_back(iterBlock);
    mBuilder->SetInsertPoint(iterBlock);
    Codegen(loop->GetIterateStmt());
    mBuilder->CreateBr(condBlock);

    // Statements following the loop will be generated in the exit block.
    function->getBasicBlockList().push_back(exitBlock);
    mBuilder->SetInsertPoint(exitBlock);
}

void 
CgStmt::Visit(IRCatchStmt* stmt, int ignored)
{
    // Create a new block for statements following the "catch" statement,
    // which will be targeted by jumps generated for return statements
    // in the body.
    stmt->mReturnTarget = llvm::BasicBlock::Create(*mContext, "return");

    // Generate code for the body, followed by a jump to the return target
    // (note that LLVM blocks never fall through).
    Codegen(stmt->GetBody());
    mBuilder->CreateBr(stmt->mReturnTarget);

    // Statements following the catch statement will be generated in the
    // return block.
    llvm::Function* function = mBuilder->GetInsertBlock()->getParent();
    function->getBasicBlockList().push_back(stmt->mReturnTarget);
    mBuilder->SetInsertPoint(stmt->mReturnTarget);
}

void 
CgStmt::Visit(IRControlStmt* stmt, int ignored)
{
    // Get the appropriate target from the corresponding loop or catch stmt.
    llvm::BasicBlock* target = NULL;
    IRStmt* enclosing = stmt->GetEnclosingStmt();
    if (IRForLoop* forLoop = UtCast<IRForLoop*>(enclosing)) {
        target = (stmt->GetOpcode() == kOpcode_Break)
            ? forLoop->mBreakTarget : forLoop->mContinueTarget;
    }
    else {
        IRCatchStmt* catchStmt = UtStaticCast<IRCatchStmt*>(enclosing);
        target = catchStmt->mReturnTarget;
    }
    
    // Generate a jump to the target block.
    mBuilder->CreateBr(target);

    // Put the insertion point in a new basic block in case there's any
    // unreachable code after the break.
    llvm::Function* function = mBuilder->GetInsertBlock()->getParent();
    llvm::BasicBlock* b = llvm::BasicBlock::Create(*mContext, "dead", function);
    mBuilder->SetInsertPoint(b);
}

void 
CgStmt::Visit(IRStmt* stmt, int ignored)
{
    assert(false && "Codegen unimplemented for this kind of statement");
}
