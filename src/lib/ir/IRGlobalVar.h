// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_GLOBAL_VAR_H
#define IR_GLOBAL_VAR_H

#include "ir/IRVar.h"

/// IR global variable (e.g. P, N).
class IRGlobalVar : public IRVar {
public:
    /// Check whether this global variable is ever written.
    bool IsWritten() const { return mIsWritten; }

    /// Construct a global variable.
    IRGlobalVar(const char* name, const IRType* type,
                IRDetail detail, const char* space, bool isWritten) :
        IRVar(kIRGlobalVar, name, type, detail, space),
        mIsWritten(isWritten)
    {
    }

    /// Destructor is virtual.
    virtual ~IRGlobalVar();

    /// Check if a value is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRValue* val) { 
        return val->GetKind() == kIRGlobalVar;
    }

private:
    bool mIsWritten;
};

#endif // ndef IR_GLOBAL_VAR_H
