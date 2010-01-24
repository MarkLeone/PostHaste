// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_ILLUMINANCE_STMT_H
#define IR_ILLUMINANCE_STMT_H

#include "ir/IRSpecialForm.h"
#include <assert.h>

/// IR of "illuminance" loop.
class IRIlluminanceLoop : public IRSpecialForm {
public:
    /// Get the category (NULL if unspecified).
    IRValue* GetCategory() const { return mCategory; }

    /// Construct IR of an "illuminance" loop, taking ownership of the given
    /// arguments.  The category can be NULL.
    IRIlluminanceLoop(IRValue* category,
                      const IRValues& args,                      
                      IRStmt* body, 
                      const IRPos& pos) :
        IRSpecialForm(kIRIlluminanceLoop, args, body, pos),
        mCategory(category)
    {
    }

    /// Destructor
    virtual ~IRIlluminanceLoop();

    /// Check if a statement is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRStmt* stmt) { 
        return stmt->GetKind() == kIRIlluminanceLoop;
    }

private:
    IRValue* mCategory;         //  possibly NULL.
};

#endif // ndef IR_ILLUMINANCE_STMT_H
