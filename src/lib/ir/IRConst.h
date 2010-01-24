// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_CONST_H
#define IR_CONST_H

#include "ir/IRValue.h"
#include <assert.h>
#include <string>

/**
   Abstract base class of IR constants, which has the following derived
   classes:
       - IRNumConst             Numeric constant (float, triple, or matrix).
       - IRNumArrayConst        Numeric array constant.
       - IRStringConst          String constant.
       - IRStringArrayConst     String array constant.
*/
class IRConst : public IRValue {
public:
    /// Get the constant name, which might be empty.
    const char* GetName() const { return mName.c_str(); }
    
    /// Construct an IR constant of the specified kind with the given type.
    /// The detail is always uniform.  The name is optional; it is copied.
    IRConst(IRValueKind kind, const IRType* type, const char* name=""):
        IRValue(kind, type, kIRUniform),
        mName(name),
        mOrigDataOffset(0)
    {
        assert(kIRConst_Begin < kind && kind < kIRConst_End &&
               "Invalid kind of constant");
    }

    /// Destructor.  Not sure why it can't be pure virtual.
    virtual ~IRConst();

    /// Check if a value is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRValue* val) { 
        IRValueKind kind = val->GetKind();
        return kIRConst_Begin < kind && kind < kIRConst_End;
    }

private:
    friend class XfRaiseImpl;
    friend class XfLowerImpl;
    std::string mName;
    int mOrigDataOffset;
};

/// Print an IR constant.
std::ostream& operator<<(std::ostream& out, const IRConst& constant);

#endif // ndef IR_CONST_H
