// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "xf/XfPartition.h"
#include "ir/IRShader.h"
#include "ops/OpInfo.h"

enum Kind { kNone, kCompiled, kInterpreted };

void 
XfPartition(IRShader* shader)
{
    XfPartitionImpl().Partition(shader);
}

void 
XfPartitionImpl::Partition(IRShader* shader)
{
    shader->SetBody(Partition(shader->GetBody()));
}

static Kind
GetKind(const IRInst* inst)
{
    // If there's no shadeop implementation, we can't compile it.
    Opcode opcode = inst->GetOpcode();
    if (OpInfo::GetOpName(opcode) == NULL)
        return kInterpreted;

    // Otherwise if the result is uniform, it should be interpreted.
    if (inst->GetResult() &&
        inst->GetResult()->GetDetail() == kIRUniform)
        return kInterpreted;

    // If any output parameter is uniform, it should be interpreted.
    if (OpInfo::HasOutput(opcode)) {
        unsigned int i = 0;
        const IRValues& args = inst->GetArgs();
        IRValues::const_iterator it;
        for (it = args.begin(); it != args.end(); ++it, ++i) {
            IRValue* arg = *it;
            if (OpInfo::IsOutput(opcode, i) && arg->GetDetail() == kIRUniform)
                return kInterpreted;
        }
    }
    return kCompiled;
}

static Kind
GetKind(const IRStmt* stmt)
{
    return stmt->CanCompile() ? kCompiled : kInterpreted;
}

static IRStmt*
MarkWithKind(Kind kind, IRStmt* stmt)
{
    if (kind == kCompiled)
        stmt->SetCanCompile();
    return stmt;
}

// Create a sequence (if necessary) from the given statements.  If there's a
// single statement, its kind determines the kind of the result.
static IRStmt*
MakeSeq(IRStmts* stmts, const IRStringConst* funcName=NULL)
{
    Kind kind = kNone;
    size_t numStmts = stmts->size();
    if (numStmts == 0)
        kind = kCompiled;
    else if (numStmts == 1)
        kind = GetKind(stmts->front());
    return MarkWithKind(kind, IRSeq::Create(stmts, funcName));
}

IRStmt*
XfPartitionImpl::Visit(IRBlock* block, int ignored)
{
    // Multiple blocks might result.
    IRStmts* resultStmts = new IRStmts;
    IRInsts* currentInsts = new IRInsts;
    Kind currentKind = kNone;
    
    // Get the instructions and destroy the original block.
    IRInsts* insts = block->TakeInsts();
    delete block;
    block = NULL;

    // Partition the instructions by kind into separate blocks.
    IRInsts::const_iterator it;
    for (it = insts->begin(); it != insts->end(); ++it) {
        // If this is a different kind of instruction, start a new block.
        IRInst* inst = *it;
        Kind kind = GetKind(inst);
        if (kind != currentKind) {
            // Wrap up the current block, if any
            if (!currentInsts->empty()) {
                resultStmts->push_back(
                        MarkWithKind(currentKind, new IRBlock(currentInsts)));
                currentInsts = new IRInsts;
            }
            currentKind = kind;
        }
        currentInsts->push_back(inst);
    }

    // Discard the original instruction list.
    delete insts;
    
    // Wrap up the current block, if any
    if (!currentInsts->empty())
        resultStmts->push_back(
                MarkWithKind(currentKind, new IRBlock(currentInsts)));
    else
        delete currentInsts;

    // If there's only one block, all the instructions had the same kind.
    // Otherwise the result is a sequence with blocks of differen kinds.
    return MakeSeq(resultStmts);
}
 
IRStmt*
XfPartitionImpl::Visit(IRSeq* seq, int ignored)
{
    // Early exit if it's an empty sequence.
    if (seq->GetStmts().empty()) {
        seq->SetCanCompile();
        return seq;
    }

    // Multiple statements might result.
    IRStmts* resultStmts = new IRStmts;
    IRStmts* currentStmts = new IRStmts;
    Kind currentKind = kNone;

    // Get the statement list and destroy the original sequence.
    IRStmts* stmts = seq->TakeStmts();
    const IRStringConst* funcName = seq->GetFuncName();
    delete seq;
    seq = NULL;

    // Partition the statements by kind into separate sequences.
    IRStmts::const_iterator it;
    for (it = stmts->begin(); it != stmts->end(); ++it) {
        // Recursively partition the statement.
        IRStmt* stmt = Partition(*it);
        
        // If this is a different kind of statement, start a new sequence.
        Kind kind = GetKind(stmt);
        if (kind != currentKind) {
            // Wrap up the current sequence, if any.
            if (!currentStmts->empty()) {
                resultStmts->push_back(
                        MarkWithKind(currentKind, IRSeq::Create(currentStmts)));
                currentStmts = new IRStmts;
            }
            currentKind = kind;
        }
        currentStmts->push_back(stmt);
    }

    // Discard the original statement list.
    delete stmts;

    // Wrap up the current sequence, if any.
    if (!currentStmts->empty())
        resultStmts->push_back(
                MarkWithKind(currentKind, IRSeq::Create(currentStmts)));
    else
        delete currentStmts;

    // If there's only one block, all the statements had the same kind.
    // Otherwise the result is a sequence containing statements
    // (sub-sequences) of different kinds.
    return MakeSeq(resultStmts, funcName);
}
 
IRStmt*
XfPartitionImpl::Visit(IRIfStmt* stmt, int ignored)
{
    stmt->SetThen(Partition(stmt->GetThen()));
    stmt->SetElse(Partition(stmt->GetElse()));
    if (stmt->GetThen()->CanCompile() && stmt->GetElse()->CanCompile())
        stmt->SetCanCompile();
    return stmt;
}

IRStmt*
XfPartitionImpl::Visit(IRForLoop* loop, int ignored)
{
    loop->SetCondStmt(Partition(loop->GetCondStmt()));
    loop->SetIterateStmt(Partition(loop->GetIterateStmt()));
    loop->SetBody(Partition(loop->GetBody()));
    if (loop->GetCondStmt()->CanCompile() && 
        loop->GetIterateStmt()->CanCompile() && loop->GetBody()->CanCompile())
        loop->SetCanCompile();
    return loop;
}

IRStmt*
XfPartitionImpl::Visit(IRControlStmt* stmt, int ignored)
{
    stmt->SetCanCompile();
    return stmt;
}

IRStmt*
XfPartitionImpl::Visit(IRGatherLoop* loop, int ignored)
{
    loop->SetBody(Partition(loop->GetBody()));
    loop->SetElseStmt(Partition(loop->GetElseStmt()));
    return loop;  // cannot compile
}

IRStmt*
XfPartitionImpl::Visit(IRIlluminanceLoop* loop, int ignored)
{
    loop->SetBody(Partition(loop->GetBody()));
    return loop;  // cannot compile
}

IRStmt*
XfPartitionImpl::Visit(IRIlluminateStmt* stmt, int ignored)
{
    stmt->SetBody(Partition(stmt->GetBody()));
    // We don't yet support compiling illuminate/solar statements, 
    // but we could.
    return stmt;
}

IRStmt*
XfPartitionImpl::Visit(IRCatchStmt* stmt, int ignored)
{
    stmt->SetBody(Partition(stmt->GetBody()));
    if (stmt->GetBody()->CanCompile())
        stmt->SetCanCompile();
    return stmt;
}

IRStmt*
XfPartitionImpl::Visit(IRPluginCall* call, int ignored)
{
    return call;
}
