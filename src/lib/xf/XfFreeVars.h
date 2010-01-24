// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef XF_FREE_VARS_H
#define XF_FREE_VARS_H

#include "ir/IRVisitor.h"
#include "ir/IRTypedefs.h"
#include "ir/IRVarSet.h"
#include <iosfwd>
class IRShader;
class IRVar;
class IRVarSet;

/// Given a partitioned shader, attach free variable sets to the root
/// statement of each partition.
void XfFreeVars(IRShader* shader);

/// Implementation of free variable analysis.  Methods are all public for
/// testing.
class XfFreeVarsImpl : public IRVisitor<XfFreeVarsImpl> {
private:
    IRVar* mGlobalL;
    IRVar* mGlobalCl;
    bool mInPartition;

public:
    class FreeVars {
    public:
        IRVarSet mInputs, mOutputs;

        // XXX Default ctor shouldn't be necessary
        FreeVars() { }

        // Not sure if this copy constructor is necessary.
        FreeVars(const FreeVars& freeVars) :
            mInputs(freeVars.mInputs),
            mOutputs(freeVars.mOutputs)
        {
        }

        void operator+=(const FreeVars& freeVars) {
            mInputs += freeVars.mInputs;
            mOutputs += freeVars.mOutputs;
        }

        void operator-=(IRVar* var) { 
            mInputs -= var;
            mOutputs -= var; 
        }

        void Prune(const IRVarSet& liveBefore, const IRVarSet& liveAfter) {
            mInputs.Intersect(liveBefore);
            mOutputs.Intersect(liveAfter);
        }
    };


    XfFreeVarsImpl();
    void Analyze(IRShader* shader);
    FreeVars Analyze(IRStmt* stmt, IRVarSet* live);
    FreeVars Analyze(const IRInst* inst, IRVarSet* live);
    FreeVars Analyze(Opcode opcode, IRVar* result, const IRValues& args,
                     IRVarSet* live);
    FreeVars AnalyzeBranch(IRStmt* thenStmt, IRStmt* elseStmt, IRVarSet* live);

    FreeVars Visit(IRBlock* stmt, IRVarSet* live);
    FreeVars Visit(IRSeq* stmt, IRVarSet* live);
    FreeVars Visit(IRIfStmt* stmt, IRVarSet* live);
    FreeVars Visit(IRForLoop* stmt, IRVarSet* live);
    FreeVars Visit(IRCatchStmt* stmt, IRVarSet* live);
    FreeVars Visit(IRControlStmt* stmt, IRVarSet* live);
    FreeVars Visit(IRGatherLoop* stmt, IRVarSet* live);
    FreeVars Visit(IRIlluminanceLoop* stmt, IRVarSet* live);
    FreeVars Visit(IRIlluminateStmt* stmt, IRVarSet* live);
    FreeVars VisitIllumBody(IRStmt* body, IRVarSet* live);
    FreeVars Visit(IRPluginCall* call, IRVarSet* live);

};

#endif // ndef XF_FREE_VARS_H
