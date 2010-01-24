// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef CG_STMT_H
#define CG_STMT_H

#include "cg/CgComponent.h"
#include "cg/CgFwd.h"
#include "ir/IRVisitor.h"
#include <map>

/// Code generation for IR statements.
class CgStmt : public CgComponent, public IRVisitor<CgStmt> {
public:
    /// Constructor
    CgStmt(const CgComponent& state) : 
        CgComponent(state)
    {
    }

    /// Generate code for an IR statement.
    void Codegen(IRStmt* stmt) const;

    void Visit(IRBlock* stmt, int ignored);
    void Visit(IRSeq* stmt, int ignored);
    void Visit(IRIfStmt* stmt, int ignored);
    void Visit(IRForLoop* stmt, int ignored);
    void Visit(IRCatchStmt* stmt, int ignored);
    void Visit(IRControlStmt* stmt, int ignored);
    // void Visit(IRSpecialForm* stmt, int ignored);
    // void Visit(IRPluginCall* stmt, int ignored);
    void Visit(IRStmt* stmt, int ignored);
};

#endif // ndef CG_STMT_H
