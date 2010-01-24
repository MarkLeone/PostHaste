// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_GATHER_STMT_H
#define IR_GATHER_STMT_H

#include "ir/IRSpecialForm.h"
#include "ir/IRLocalVar.h"
#include <assert.h>

/// IR of "gather" loop.
class IRGatherLoop : public IRSpecialForm {
public:
    /// Get the category (never NULL).
    IRValue* GetCategory() const { return mCategory; }

    /// Get the else branch (never NULL).
    IRStmt* GetElseStmt() const { return mElseStmt; }

    /// Set the else branch, which must not be NULL.
    void SetElseStmt(IRStmt* stmt) {
        assert(stmt && "Else branch of gather loop must not be NULL");
        mElseStmt = stmt;
    }

    /// Construct IR of an "gather" loop, taking ownership of the given
    /// arguments.  The category cannot be NULL.  Skeletal loops are temporarily
    /// constructed with NULL children in XfRaise, but otherwise the child
    /// statements are never NULL.

    IRGatherLoop(IRValue* category, 
                 const IRValues& args, 
                 IRStmt* body, 
                 IRStmt* elseStmt,
                 const IRPos& pos) :
        IRSpecialForm(kIRGatherLoop, args, body, pos),
        mCategory(category),
        mElseStmt(elseStmt),
        mMarker(NULL)
    {
        assert(mCategory && "Gather category is non-optional");
    }

    /// The destructor deletes the children.
    virtual ~IRGatherLoop();

    /// Check if a statement is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRStmt* stmt) { 
        return stmt->GetKind() == kIRGatherLoop;
    }

private:
    IRValue* mCategory;
    IRStmt* mElseStmt;

    // XfRaise stashes the loop's "marker" variable for reuse by XfLower.
    friend class XfRaiseImpl;
    friend class XfLowerImpl;
    IRLocalVar* mMarker;
};

#endif // ndef IR_GATHER_STMT_H
