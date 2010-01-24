// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "xf/XfFreeVars.h"
#include "ir/IRGlobalVar.h"
#include "ir/IRShader.h"
#include "ir/IRShaderParam.h"
#include "ir/IRGlobalVar.h"
#include "ir/IRVarSet.h"
#include "ops/OpInfo.h"
#include "util/UtCast.h"
#include <iostream>

// Given a partitioned shader, attach a free variable set to the root
// statement of each partition.
void 
XfFreeVars(IRShader* shader)
{
    XfFreeVarsImpl().Analyze(shader);
}

XfFreeVarsImpl::XfFreeVarsImpl() :
    mGlobalL(NULL),
    mGlobalCl(NULL),
    mInPartition(false)
{
}

void
XfFreeVarsImpl::Analyze(IRShader* shader)
{
    // Output shader parameters are live after the shader executes.
    IRVarSet live;
    const IRShaderParams& params = shader->GetParams();
    IRShaderParams::const_iterator param;
    for (param = params.begin(); param != params.end(); ++param)
        if ((*param)->IsOutput())
            live += *param;

    // Globals are also live afterwards.  Keep track of the global L and Cl
    // variables (if present), since they have special lifetimes.
    const IRGlobalVars& globals = shader->GetGlobals();
    IRGlobalVars::const_iterator g;
    for (g = globals.begin(); g != globals.end(); ++g) {
        IRGlobalVar* global = *g;
        live += global;
        if (!strcmp(global->GetFullName(), "L"))
            mGlobalL = global;
        else if (!strcmp(global->GetFullName(), "Cl"))
            mGlobalCl = global;
    }

    // Analyze the body of the shader, attaching free variable set to the root
    // statement of each partition.
    Analyze(shader->GetBody(), &live);
}

XfFreeVarsImpl::FreeVars 
XfFreeVarsImpl::Analyze(IRStmt* stmt, IRVarSet* live)
{
    // Check whether this statement is the root of a compilable partition.
    bool isPartition = !mInPartition && stmt->CanCompile();
    if (isPartition)
        mInPartition = true;

    // Analyze the statement.
    FreeVars freeVars = Dispatch<FreeVars>(const_cast<IRStmt*>(stmt), live); 

    // If this is a partition root, attach a set of free variables.  Note loop
    // bodies are analyzed twice, so any existing free variable set is
    // discarded.
    if (isPartition) {
        delete stmt->TakeFreeVars();
        IRVarSet* freeSet = new IRVarSet(freeVars.mInputs);
        *freeSet += freeVars.mOutputs;
        stmt->SetFreeVars(freeSet);
        mInPartition = false;
    }
    return freeVars;
}

XfFreeVarsImpl::FreeVars 
XfFreeVarsImpl::Analyze(const IRInst* inst, IRVarSet* live)
{
    return Analyze(inst->GetOpcode(), inst->GetResult(), inst->GetArgs(), live);
}

// Remove the given variable from the live variable set, unless it's a global
// or a shader output parameter.
static void
Kill(const IRVar* var, IRVarSet* live)
{
    if (const IRShaderParam* param = UtCast<const IRShaderParam*>(var)) {
        if (!param->IsOutput())
            *live -= var;
    }
    else if (!UtIsInstance<const IRGlobalVar*>(var))
        *live -= var;
}


// Add any variables in the argument list to the live variable set, unless
// they're outputs that are killed (i.e. value ignored on input; completely
// overwritten on output).  Returns the sets of input and output variables.
XfFreeVarsImpl::FreeVars
XfFreeVarsImpl::Analyze(Opcode opcode, IRVar* result, const IRValues& args,
                        IRVarSet* live)
{
    FreeVars freeVars;

    // Kill the result variable (if any).  It might be added back if it's
    // referenced in the arguments.
    if (result) {
        Kill(result, live);
        freeVars.mOutputs += result;
    }

    // Extra checking is required if the instruction has output arguments.
    bool hasOutput = OpInfo::HasOutput(opcode);

    IRValues::const_iterator it;
    int i = 0;
    for (it = args.begin(); it != args.end(); ++it, ++i) {
        if (IRVar* var = UtCast<IRVar*>(*it)) {
            if (hasOutput && OpInfo::IsOutput(opcode, i)) {
                freeVars.mOutputs += var;
                if (OpInfo::KillsArg(opcode, i))
                    Kill(var, live);
                else {
                    *live += var;
                    freeVars.mInputs += var;
                }
            }
            else {
                *live += var;
                freeVars.mInputs += var;
            }
        }
    }
    return freeVars;
}

XfFreeVarsImpl::FreeVars
XfFreeVarsImpl::Visit(IRBlock* stmt, IRVarSet* live)
{
    FreeVars freeVars;
    IRVarSet liveAfter = *live;
    const IRInsts& insts = stmt->GetInsts();
    IRInsts::const_reverse_iterator it;
    for (it = insts.rbegin(); it != insts.rend(); ++it) {
        freeVars += Analyze(*it, live);
    }
    freeVars.Prune(*live, liveAfter);
    return freeVars;
}

XfFreeVarsImpl::FreeVars
XfFreeVarsImpl::Visit(IRSeq* seq, IRVarSet* live)
{
    FreeVars freeVars;
    IRVarSet liveAfter = *live;
    const IRStmts& stmts = seq->GetStmts();
    IRStmts::const_reverse_iterator it;
    for (it = stmts.rbegin(); it != stmts.rend(); ++it) {
        freeVars += Analyze(*it, live);
    }
    freeVars.Prune(*live, liveAfter);
    return freeVars;
}

XfFreeVarsImpl::FreeVars
XfFreeVarsImpl::AnalyzeBranch(IRStmt* thenStmt, IRStmt* elseStmt,
                              IRVarSet* live)
{
    // Compute live variables in the "else" branch using a copy of the
    // variables that are live after the "if" statement.
    IRVarSet liveElse = *live;
    FreeVars freeVars = Analyze(elseStmt, &liveElse);

    // Compute the live variables in the "then" branch and union those from
    // the "else" branch.
    freeVars += Analyze(thenStmt, live);
    *live += liveElse;
    return freeVars;
}

XfFreeVarsImpl::FreeVars
XfFreeVarsImpl::Visit(IRIfStmt* stmt, IRVarSet* live)
{
    FreeVars freeVars = AnalyzeBranch(stmt->GetThen(), stmt->GetElse(), live);
    *live += stmt->GetCond();
    freeVars.mInputs += stmt->GetCond();
    return freeVars;
}

XfFreeVarsImpl::FreeVars
XfFreeVarsImpl::Visit(IRForLoop* loop, IRVarSet* live)
{
    IRVarSet liveAfter = *live;

    // The loop might not execute at all, except for the condition statement,
    // so we compute the live variables of the "iterate" statement and the
    // loop body in a copy of the set of variables that are live after the
    // loop.  Note that the loop body might kill variables that are live in
    // the iterate statement.
    IRVarSet liveInLoop = *live;
    Analyze(loop->GetIterateStmt(), &liveInLoop);
    Analyze(loop->GetBody(), &liveInLoop);

    // Add the condition variable and compute live variables in the condition
    // statement (which might kill the condition variable).
    *live += loop->GetCond();
    Analyze(loop->GetCondStmt(), &liveInLoop);

    // Variables from one iteration (including the loop condition) are live in
    // the previous iteration, so we must re-analyze the loop.
    FreeVars freeVars = Analyze(loop->GetIterateStmt(), &liveInLoop);
    freeVars += Analyze(loop->GetBody(), &liveInLoop);

    // Union the "live after" set with the loop's live variables, since
    // the loop might not execute at all, and then add the loop condition.
    *live += liveInLoop;
    *live += loop->GetCond();
    freeVars.mInputs += loop->GetCond();
    freeVars += Analyze(loop->GetCondStmt(), live);
    freeVars.Prune(*live, liveAfter);
    return freeVars;
}

XfFreeVarsImpl::FreeVars
XfFreeVarsImpl::Visit(IRCatchStmt* stmt, IRVarSet* live)
{
    return Analyze(stmt->GetBody(), live);
}

XfFreeVarsImpl::FreeVars
XfFreeVarsImpl::Visit(IRControlStmt* stmt, IRVarSet* live)
{
    // An accurate answer would include only those variables that are live in
    // the target of the control statement.  But we don't have that info, so
    // we conservatively do nothing.
    return FreeVars();
}

XfFreeVarsImpl::FreeVars
XfFreeVarsImpl::Visit(IRGatherLoop* loop, IRVarSet* live)
{
    IRVarSet liveAfter = *live;

    // The loop body might not execute, so we analyze it with a copy of the
    // set of variables that are live after the loop and later union them.
    IRVarSet liveInLoop = *live;

    // Get the union of the live variables in the loop body and the "else"
    // branch.
    AnalyzeBranch(loop->GetBody(), loop->GetElseStmt(), &liveInLoop);

    // Variables that are live in one iteration are live in the previous
    // iteration, so we must re-analyze the loop.
    FreeVars freeVars = 
        AnalyzeBranch(loop->GetBody(), loop->GetElseStmt(), &liveInLoop);

    // Union the loop's live variables with those that are live afterwards,
    // since the loop might not execute at all.
    *live += liveInLoop;

    // Add the gather arguments.  Some are output arguments (e.g. hit color
    // might be fetched), but they're conditionally assigned, not
    // unequivocally killed.
    freeVars += Analyze(kOpcode_Gather, NULL, loop->GetArgs(), live);
    *live += loop->GetCategory();
    freeVars.mInputs += loop->GetCategory();
    freeVars.Prune(*live, liveAfter);
    return freeVars;
}

XfFreeVarsImpl::FreeVars
XfFreeVarsImpl::VisitIllumBody(IRStmt* body, IRVarSet* live)
{
    // Compute the live variables in the loop body, then remove L and Cl.
    FreeVars freeVars = Analyze(body, live);
    if (mGlobalL) {
        *live -= mGlobalL;
        freeVars -= mGlobalL;
    }
    if (mGlobalCl) {
        *live -= mGlobalCl;
        freeVars -= mGlobalCl;
    }
    return freeVars;
}

XfFreeVarsImpl::FreeVars
XfFreeVarsImpl::Visit(IRIlluminanceLoop* loop, IRVarSet* live)
{
    IRVarSet liveAfter = *live;

    // The loop body might not execute, so we analyze it with a copy of the
    // set of variables that are live after the loop and later union them.
    IRVarSet liveInLoop = *live;

    // Variables that are live in one iteration are live in the previous
    // iteration, so we must analyze the loop body twice.
    VisitIllumBody(loop->GetBody(), &liveInLoop);
    FreeVars freeVars = VisitIllumBody(loop->GetBody(), &liveInLoop);

    // Union the loop's live variables with those that are live afterwards,
    // since the loop might not execute at all.
    *live += liveInLoop;

    // Add the illuminance arguments.  Some are output arguments (e.g. light
    // parameters migh be fetched), but they're conditionally assigned, not
    // unequivocally killed.
    freeVars += Analyze(kOpcode_Illuminance, NULL, loop->GetArgs(), live);
    *live += loop->GetCategory();
    freeVars.mInputs += loop->GetCategory();
    freeVars.Prune(*live, liveAfter);
    return freeVars;
}

XfFreeVarsImpl::FreeVars
XfFreeVarsImpl::Visit(IRIlluminateStmt* stmt, IRVarSet* live)
{
    FreeVars freeVars = VisitIllumBody(stmt->GetBody(), live);
    freeVars += Analyze(kOpcode_Illuminate, NULL, stmt->GetArgs(), live);
    return freeVars;
}

XfFreeVarsImpl::FreeVars
XfFreeVarsImpl::Visit(IRPluginCall* call, IRVarSet* live)
{
    return Analyze(kOpcode_CallDSO, NULL, call->GetArgs(), live);
}
