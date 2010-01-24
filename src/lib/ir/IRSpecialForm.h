// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_SPECIAL_FORM_H
#define IR_SPECIAL_FORM_H

#include "ir/IRStmt.h"
#include "ir/IRTypedefs.h"
#include <assert.h>
class IRValue;

/**
   Abstract base class of IR for RSL special forms.  Derived classes are:
      - IRGatherLoop
      - IRIlluminanceLoop
      - IRIlluminateStmt
*/
class IRSpecialForm : public IRStmt {
public:
    /// Get the arguments (e.g. axis, angle, etc.).  Excludes the category,
    /// which is present only in gather and illuminance loops.
    const IRValues& GetArgs() const { return mArgs; }

    /// Get the loop body (never NULL).
    IRStmt* GetBody() const { return mBody; }

    /// Set the loop body, which must not be NULL.
    void SetBody(IRStmt* stmt) {
        assert(stmt && "Loop body cannot be NULL");
        mBody = stmt;
    }

    /// Construct loop IR, taking ownership of the loop body (but not the
    /// arguments).  A skeletal form with a NULL body might be temporarily
    /// constructed by XfRaise, but otherwise the body is never NULL.
    IRSpecialForm(IRStmtKind kind,
                const IRValues& args, 
                IRStmt* body, 
                const IRPos& pos) :
        IRStmt(kind, pos),
        mArgs(args),
        mBody(body)
    {
        assert((kind == kIRGatherLoop || kind == kIRIlluminanceLoop ||
                kind == kIRIlluminateStmt) && "Invalid kind of special form");
    }

    /// The destructor deletes the loop body.
    virtual ~IRSpecialForm();

    /// Check if a statement is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRStmt* stmt) { 
        IRStmtKind kind = stmt->GetKind();
        return kind == kIRGatherLoop || kind == kIRIlluminanceLoop ||
            kind == kIRIlluminateStmt;
    }

private:
    IRValues mArgs;
    IRStmt* mBody;
};

#endif // ndef IR_SPECIAL_FORM_H
