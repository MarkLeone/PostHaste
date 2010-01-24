// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_STRING_CONST_H
#define IR_STRING_CONST_H

#include "ir/IRConst.h"
#include "ir/IRTypes.h"
#include <string>
#include <iosfwd>

/// IR for a string constant.
class IRStringConst : public IRConst {
public:
    /// Get the value of the string constant.
    const char* Get() const { return mString.c_str(); }

    /// Construct a string constant, copying the given string.  The name is
    /// optional; it is copied.
    explicit IRStringConst(const char* string, const char* name="") :
        IRConst(kIRStringConst, IRTypes::GetStringTy(), name),
        mString(string)
    {
    }

    /// Check if a value is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRValue* val) { 
        return val->GetKind() == kIRStringConst;
    }

private:
    std::string mString;
};

/// Print a string constant.
std::ostream& operator<<(std::ostream& out, const IRStringConst& constant);

#endif // ndef IR_STRING_CONST_H
