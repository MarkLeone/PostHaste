// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "xf/XfLiveVars.h"
#include "ir/IRGlobalVar.h"
#include "ir/IRVarSet.h"
#include "ops/OpInfo.h"
#include "util/UtCast.h"

IRVarSet
XfLiveVars(const IRStmt* stmt)
{
    // Note that we use an empty list of global variables, so L and Cl won't
    // be properly killed if the statement contains and illuminance/illuminate
    // loop.
    IRGlobalVars noGlobals;
    IRVarSet live;
    XfLiveVarsImpl(noGlobals).GetLive(stmt, &live);
    return live;
}

// Constructor
XfLiveVarsImpl::XfLiveVarsImpl(const IRGlobalVars& globals) :
    mGlobalL(NULL),
    mGlobalCl(NULL)
{
    IRGlobalVars::const_iterator it;
    for (it = globals.begin(); it != globals.end(); ++it) {
        IRGlobalVar* global = *it;
        if (!strcmp(global->GetFullName(), "L"))
            mGlobalL = global;
        else if (!strcmp(global->GetFullName(), "Cl"))
            mGlobalCl = global;
    }
}

void 
XfLiveVarsImpl::GetLive(const IRStmt* stmt, IRVarSet* live)
{
    Dispatch<void>(const_cast<IRStmt*>(stmt), live); 
}

void 
XfLiveVarsImpl::GetLive(const IRInst* inst, IRVarSet* live)
{
    // Kill the result variable, if any.
    if (inst->GetResult())
        *live -= inst->GetResult();

    // Extra checking is required if the instruction has output arguments.
    Opcode opcode = inst->GetOpcode();
    bool noOutputs = !OpInfo::HasOutput(opcode);

    // Add any variables in the argument list to the live variable set, unless
    // they're outputs that are killed (i.e. value ignored on input;
    // completely overwritten on output).
    const IRValues& args = inst->GetArgs();
    IRValues::const_iterator it;
    int i = 0;
    for (it = args.begin(); it != args.end(); ++it, ++i) {
        if (noOutputs || !OpInfo::KillsArg(opcode, i))
            *live += *it;
    }
}

void
XfLiveVarsImpl::Visit(IRBlock* stmt, IRVarSet* live)
{
    const IRInsts& insts = stmt->GetInsts();
    IRInsts::const_reverse_iterator it;
    for (it = insts.rbegin(); it != insts.rend(); ++it) {
        GetLive(*it, live);
    }
}

void
XfLiveVarsImpl::Visit(IRSeq* seq, IRVarSet* live)
{
    const IRStmts& stmts = seq->GetStmts();
    IRStmts::const_reverse_iterator it;
    for (it = stmts.rbegin(); it != stmts.rend(); ++it) {
        GetLive(*it, live);
    }
}

void
XfLiveVarsImpl::GetLiveBranch(const IRStmt* thenStmt, const IRStmt* elseStmt, 
                              IRVarSet* live)
{
    // Compute live variables in the "else" branch using a copy of the
    // variables that are live after the "if" statement.
    IRVarSet liveElse = *live;
    GetLive(elseStmt, &liveElse);

    // Compute the live variables in the "then" branch and union those from
    // the "else" branch.
    GetLive(thenStmt, live);
    *live += liveElse;
}

void
XfLiveVarsImpl::Visit(IRIfStmt* stmt, IRVarSet* live)
{
    GetLiveBranch(stmt->GetThen(), stmt->GetElse(), live);
    *live += stmt->GetCond();
}

void
XfLiveVarsImpl::Visit(IRForLoop* loop, IRVarSet* live)
{
    // The loop might not execute at all, except for the condition statement,
    // so we compute the live variables of the "iterate" statement and the
    // loop body in a copy of the set of variables that are live after the
    // loop.  Note that the loop body might kill variables that are live in
    // the iterate statement.
    IRVarSet liveInLoop = *live;
    GetLive(loop->GetIterateStmt(), &liveInLoop);
    GetLive(loop->GetBody(), &liveInLoop);

    // Add the condition variable and compute live variables in the condition
    // statement (which might kill the condition variable).
    *live += loop->GetCond();
    GetLive(loop->GetCondStmt(), &liveInLoop);

    // Variables from one iteration (including the loop condition) are live in
    // the previous iteration, so we must re-analyze the loop.
    GetLive(loop->GetIterateStmt(), &liveInLoop);
    GetLive(loop->GetBody(), &liveInLoop);

    // Union the "live after" set with the loop's live variables, since
    // the loop might not execute at all, and then add the loop condition.
    *live += liveInLoop;
    *live += loop->GetCond();
    GetLive(loop->GetCondStmt(), live);
}

void
XfLiveVarsImpl::Visit(IRCatchStmt* stmt, IRVarSet* live)
{
    GetLive(stmt->GetBody(), live);
}

void
XfLiveVarsImpl::Visit(IRControlStmt* stmt, IRVarSet* live)
{
    // An accurate answer would include only those variables that are live in
    // the target of the control statement.  But we don't have that info, so
    // we conservatively do nothing.
}

void
XfLiveVarsImpl::Visit(IRGatherLoop* loop, IRVarSet* live)
{
    // The loop body might not execute, so we analyze it with a copy of the
    // set of variables that are live after the loop and later union them.
    IRVarSet liveInLoop = *live;

    // Get the union of the live variables in the loop body and the "else"
    // branch.
    GetLiveBranch(loop->GetBody(), loop->GetElseStmt(), &liveInLoop);

    // Variables that are live in one iteration are live in the previous
    // iteration, so we must re-analyze the loop.
    GetLiveBranch(loop->GetBody(), loop->GetElseStmt(), &liveInLoop);

    // Union the loop's live variables with those that are live afterwards,
    // since the loop might not execute at all.
    *live += liveInLoop;

    // Add the gather arguments.  Some are output arguments (e.g. hit color
    // might be fetched), but they're conditionally assigned, not
    // unequivocally killed.
    *live += loop->GetArgs();
    *live += loop->GetCategory();
}

void
XfLiveVarsImpl::VisitIllumBody(IRStmt* body, IRVarSet* live)
{
    // Compute the live variables in the loop body, then remove L and Cl.
    GetLive(body, live);
    if (mGlobalL)
        *live -= mGlobalL;
    if (mGlobalCl)
        *live -= mGlobalCl;
}

void
XfLiveVarsImpl::Visit(IRIlluminanceLoop* loop, IRVarSet* live)
{
    // The loop body might not execute, so we analyze it with a copy of the
    // set of variables that are live after the loop and later union them.
    IRVarSet liveInLoop = *live;

    // Variables that are live in one iteration are live in the previous
    // iteration, so we must analyze the loop body twice.
    VisitIllumBody(loop->GetBody(), &liveInLoop);
    VisitIllumBody(loop->GetBody(), &liveInLoop);

    // Union the loop's live variables with those that are live afterwards,
    // since the loop might not execute at all.
    *live += liveInLoop;

    // Add the illuminance arguments.  Some are output arguments (e.g. light
    // parameters migh be fetched), but they're conditionally assigned, not
    // unequivocally killed.
    *live += loop->GetArgs();
    *live += loop->GetCategory();
}

void
XfLiveVarsImpl::Visit(IRIlluminateStmt* stmt, IRVarSet* live)
{
    VisitIllumBody(stmt->GetBody(), live);
    *live += stmt->GetArgs();
}

void
XfLiveVarsImpl::Visit(IRPluginCall* call, IRVarSet* live)
{
    *live -= call->GetResult();
    *live += call->GetArgs();
}
