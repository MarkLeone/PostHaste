// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_LOCAL_VAR_H
#define IR_LOCAL_VAR_H

#include "ir/IRVar.h"

/// IR local variable.
class IRLocalVar : public IRVar {
public:
    /// Construct a local variable.
    IRLocalVar(const char* name, const IRType* type,
               IRDetail detail, const char* space) :
        IRVar(kIRLocalVar, name, type, detail, space)
    {
    }

    /// Destructor is virtual.
    virtual ~IRLocalVar();

    /// Check if a value is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRValue* val) { 
        return val->GetKind() == kIRLocalVar;
    }
};

#endif // ndef IR_LOCAL_VAR_H
