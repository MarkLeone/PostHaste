// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_IF_STMT_H
#define IR_IF_STMT_H

#include "ir/IRStmt.h"
#include <assert.h>
class IRValue;

/// IR for "if" statement.
class IRIfStmt : public IRStmt {
public:
    /// Get the branch condition, which is an IRValue.
    IRValue* GetCond() const { return mCond; }

    /// Get the "then" branch, which is an IRStmt.  Never NULL.
    IRStmt* GetThen() const { return mThen; }

    /// Get the "else branch, which is an IRStmt.  Never NULL.
    IRStmt* GetElse() const { return mElse; }

    /// Set the "then" branch, which must not be NULL.
    void SetThen(IRStmt* stmt)  { 
        assert(stmt && "Branch of 'if' statement cannot be NULL");
        mThen = stmt;
    }

    /// Set the "else" branch, which must not be NULL.
    void SetElse(IRStmt* stmt)  { 
        assert(stmt && "Branch of 'if' statement cannot be NULL");
        mElse = stmt;
    }
    /// Construct IR for "if" statement, taking ownership of the given
    /// arguments.  The "else" statement must not be NULL.
    IRIfStmt(IRValue* cond, IRStmt* thenStmt, IRStmt* elseStmt, 
             const IRPos& pos) :
        IRStmt(kIRIfStmt, pos),
        mCond(cond),
        mThen(thenStmt),
        mElse(elseStmt)
    {
        assert(mCond && mThen && mElse && 
               "Children of 'if' statement cannot be NULL");
    }

    /// The destructor deletes the children.
    virtual ~IRIfStmt();

    /// Check if a statement is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRStmt* stmt) { 
        return stmt->GetKind() == kIRIfStmt;
    }

private:
    IRValue* mCond;
    IRStmt* mThen;
    IRStmt* mElse;              // possibly NULL
};

#endif // ndef IR_IF_STMT_H
