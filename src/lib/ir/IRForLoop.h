// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_FOR_STMT_H
#define IR_FOR_STMT_H

#include "ir/IRStmt.h"
#include "ir/IRLocalVar.h"
#include <assert.h>
class IRValue;
namespace llvm { class BasicBlock; }

/// IR of "for" loop.
class IRForLoop : public IRStmt {
public:
    /// Get the statement that computes the loop condition (never NULL).
    IRStmt* GetCondStmt() const { return mCondStmt; }

    /// Set the condition statement, which must not be NULL.
    void SetCondStmt(IRStmt* stmt) {
        assert(stmt != NULL && "Loop condition statement must not be NULL");
        mCondStmt = stmt;
    }

    /// Get the loop condition, which is an IRValue (never NULL).
    IRValue* GetCond() const { return mCond; }

    /// Get the iteration statement (e.g. increment of loop counter).
    /// Never NULL.
    IRStmt* GetIterateStmt() const { return mIterateStmt; }

    /// Set the iteration statement, which must not be NULL.
    void SetIterateStmt(IRStmt* stmt) { 
        assert(stmt != NULL && "Loop iterate statement must not be NULL");
        mIterateStmt = stmt;
    }

    /// Get the loop body, which is an IRStmt (never NULL).
    IRStmt* GetBody() const { return mBody; }

    /// Set the loop body, which must not be NULL.
    void SetBody(IRStmt* stmt) {
        assert(stmt != NULL && "Loop body must not be NULL");
        mBody = stmt;
    }


    /// Construct IR of "for" loop, taking ownership of the given arguments.
    /// Skeletal loops are temporarily constructed with NULL children in
    /// XfRaise, but otherwise the children are never NULL.
    IRForLoop(IRStmt* condStmt, IRValue* cond, IRStmt* iterateStmt,
              IRStmt* body, const IRPos& pos) :
        IRStmt(kIRForLoop, pos),
        mCondStmt(condStmt),
        mCond(cond),
        mIterateStmt(iterateStmt),
        mBody(body),
        mMarker(NULL),
        mBreakTarget(NULL),
        mContinueTarget(NULL)
    {
    }

    /// The destructor deletes the child statents (but not the values).
    virtual ~IRForLoop();

    /// Check if a statement is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRStmt* stmt) { 
        return stmt->GetKind() == kIRForLoop;
    }

private:
    IRStmt* mCondStmt;
    IRValue* mCond;
    IRStmt* mIterateStmt;
    IRStmt* mBody;
    
    // XfRaise stashes the loop's "marker" variable for reuse by XfLower.
    friend class XfRaiseImpl;
    friend class XfLowerImpl;
    IRLocalVar* mMarker;

    // The code generator records the basic blocks for the loop exit and the
    // iterate statement, which is targeted by break/continue statements.
    friend class CgStmt;
    llvm::BasicBlock* mBreakTarget;
    llvm::BasicBlock* mContinueTarget;
};

#endif // ndef IR_FOR_STMT_H
