// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_ILLUMINATE_STMT_H
#define IR_ILLUMINATE_STMT_H

#include "ir/IRSpecialForm.h"

/// IR of "illuminate" stmt.
class IRIlluminateStmt : public IRSpecialForm {
public:
    /// Check whether this is a "solar" illuminate stmt.
    bool IsSolar() const { return mIsSolar; }

    /// Construct IR of an "illuminate" stmt, taking ownership of the given
    /// arguments.
    IRIlluminateStmt(bool isSolar,
                     const IRValues& args,                      
                     IRStmt* body, 
                     const IRPos& pos) :
        IRSpecialForm(kIRIlluminateStmt, args, body, pos),
        mIsSolar(isSolar)
    {
    }

    /// The destructor deletes the children.
    virtual ~IRIlluminateStmt();

    /// Check if a statement is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRStmt* stmt) { 
        return stmt->GetKind() == kIRIlluminateStmt;
    }

private:
    bool mIsSolar;
};

#endif // ndef IR_ILLUMINATE_STMT_H
