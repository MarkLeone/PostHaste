// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_CONTROL_STMT_H
#define IR_CONTROL_STMT_H

#include "ir/IRStmt.h"
#include "ir/IRVar.h"
#include "ops/Opcode.h"
#include <assert.h>
class IRValue;

/// IR for a control-flow statement, i.e. break, continue, or return.
class IRControlStmt : public IRStmt {
public:
    /// Get the opcode, which indicates whether it's a break, continue,
    /// or return.
    Opcode GetOpcode() const { return mOpcode; }

    /// Get the enclosing loop or "catch" statement associated with this
    /// control statement.
    IRStmt* GetEnclosingStmt() const { return mEnclosingStmt; }

    /// Construct IR for a control-flow statement.
    IRControlStmt(Opcode opcode, IRStmt* enclosingStmt, const IRPos& pos);

    /// The destructor is a no-op.
    virtual ~IRControlStmt();

    /// Check if a statement is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRStmt* stmt) { 
        return stmt->GetKind() == kIRControlStmt;
    }

private:
    Opcode mOpcode;
    IRStmt* mEnclosingStmt;
};

#endif // ndef IR_CONTROL_STMT_H
