// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_SEQ_H
#define IR_SEQ_H

#include "ir/IRStmt.h"
#include "ir/IRTypedefs.h"
class IRStringConst;

/// A sequence is an IR statement that contains a sequence of IR statements.
class IRSeq : public IRStmt {
public:
    /// Get the sequence of statements.
    const IRStmts& GetStmts() const { return *mStmts; }

    /// Get a non-const reference to the sequence of statements.
    IRStmts& GetStmts() { return *mStmts; }

    /// Get the statements, taking ownership.
    IRStmts* TakeStmts()
    {
        IRStmts* stmts = mStmts;
        mStmts = new IRStmts;
        return stmts;
    }

    /// If this sequence wraps an inlined function call, get the function
    /// name.  Otherwise returns NULL.
    const IRStringConst* GetFuncName() const { return mFuncName; }

    /// Set the position of the sequence.  This is used to record the position
    /// of "enterproc" instructions in XfRaise.
    void SetPos(const IRPos& pos) { mPos = pos; }

    /// Construct an empty sequence.
    IRSeq() : 
        IRStmt(kIRSeq, IRPos()),
        mStmts(new IRStmts),
        mFuncName(NULL)
    {
    }

    /// Construct a statement sequence, taking ownership of the given
    /// sequence.  If a function name is specified, the sequence represents
    /// the body of an inlined function call.
    IRSeq(IRStmts* stmts, const IRStringConst* funcName = NULL, 
          const IRPos& pos = IRPos()) :
        IRStmt(kIRSeq, stmts->empty() ? IRPos() : stmts->front()->GetPos()),
        mStmts(stmts),
        mFuncName(funcName)
    {
    }

    /// The destructor recursively deletes the children.
    virtual ~IRSeq();

    /// Construct a sequence (if necessary) from the given statements, taking
    /// ownership.  If the sequence has a single statement and no function
    /// name is specified, a new IRSeq is not constructed.
    static IRStmt* Create(IRStmts* stmts, const IRStringConst* funcName=NULL) 
    {
        IRStmts::const_iterator it = stmts->begin();
        if (funcName == NULL && it != stmts->end() && ++it == stmts->end()) {
            // It's a single statement, so a new sequence is not necessary.
            IRStmt* result = *stmts->begin();
            delete stmts;
            return result;
        }
        else {
            return new IRSeq(stmts, funcName);
        }
    }

    /// Check if a statement is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRStmt* stmt) { 
        return stmt->GetKind() == kIRSeq;
    }
    
private:
    IRStmts* mStmts;
    const IRStringConst* mFuncName;
};

#endif // ndef IR_SEQ_H
