// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_CATCH_STMT_H
#define IR_CATCH_STMT_H

#include "ir/IRStmt.h"
#include "ir/IRLocalVar.h"
#include <assert.h>
class IRValue;
namespace llvm { class BasicBlock; }

/// IR for a "catch" statement, which represents an inlined function call
/// that contains a nested return.
class IRCatchStmt : public IRStmt {
public:
    /// Get the body (never NULL).
    IRStmt* GetBody() const { return mBody; }

    /// Set the body, which must not be NULL.
    void SetBody(IRStmt* stmt) {
        assert(stmt != NULL && "Catch statement body must not be NULL");
        mBody = stmt;
    }

    /// Construct IR for a "catch" statement, which represents an inlined
    /// function call that contains a nested return.  A skeletal catch
    /// statement with a NULL body is temporarily constructed in XfRaise, but
    /// otherwise the body is never NULL.
    IRCatchStmt(IRStmt* body, const IRPos& pos) :
        IRStmt(kIRCatchStmt, pos),
        mBody(body),
        mMarker(NULL),
        mReturnTarget(NULL)
    {
    }

    /// The destructor deletes the children.
    virtual ~IRCatchStmt();

    /// Check if a statement is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRStmt* stmt) { 
        return stmt->GetKind() == kIRCatchStmt;
    }

private:
    IRStmt* mBody;

    // XfRaise stashes the loop's "marker" variable for reuse by XfLower.
    friend class XfRaiseImpl;
    friend class XfLowerImpl;
    IRLocalVar* mMarker;

    // The code generator records the basic block the catch statement,
    // which is targeted by return statements.
    friend class CgStmt;
    llvm::BasicBlock* mReturnTarget;
};

#endif // ndef IR_CATCH_STMT_H
