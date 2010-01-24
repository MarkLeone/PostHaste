// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef XF_LIVE_VARS_H
#define XF_LIVE_VARS_H

#include "ir/IRVisitor.h"
#include "ir/IRTypedefs.h"
#include "ir/IRVarSet.h"
class IRShader;
class IRVar;
class IRVarSet;

/// Determine the set of variables that are live at the start of the given
/// statement.
IRVarSet XfLiveVars(const IRStmt* stmt);

/// Implementation of live variable analysis.  Methods are all public for
/// testing.
class XfLiveVarsImpl : public IRVisitor<XfLiveVarsImpl> {
private:
    IRVar* mGlobalL;
    IRVar* mGlobalCl;

public:
    XfLiveVarsImpl(const IRGlobalVars& globals);
    void GetLive(const IRStmt* stmt, IRVarSet* live);
    void GetLive(const IRInst* inst, IRVarSet* live);
    void GetLiveBranch(const IRStmt* thenStmt, 
                       const IRStmt* elseStmt, IRVarSet* live);

    void Visit(IRBlock* stmt, IRVarSet* live);
    void Visit(IRSeq* stmt, IRVarSet* live);
    void Visit(IRIfStmt* stmt, IRVarSet* live);
    void Visit(IRForLoop* stmt, IRVarSet* live);
    void Visit(IRCatchStmt* stmt, IRVarSet* live);
    void Visit(IRControlStmt* stmt, IRVarSet* live);
    void Visit(IRGatherLoop* stmt, IRVarSet* live);
    void Visit(IRIlluminanceLoop* stmt, IRVarSet* live);
    void Visit(IRIlluminateStmt* stmt, IRVarSet* live);
    void VisitIllumBody(IRStmt* body, IRVarSet* live);
    void Visit(IRPluginCall* call, IRVarSet* live);
};

#endif // ndef XF_LIVE_VARS_H
