// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_STRING_ARRAY_CONST_H
#define IR_STRING_ARRAY_CONST_H

#include "ir/IRStringConst.h"
#include "ir/IRArrayType.h"
#include <assert.h>

// IR for a string array constant.
class IRStringArrayConst : public IRConst {
public:
    /// Get the array length.
    unsigned int GetLength() const 
    {
        return static_cast<unsigned int>(mStrings.size()); 
    }

    /// Get the ith string.
    const char* GetElement(unsigned int i) const
    { 
        return mStrings[i].c_str();
    }

    /// Construct a string array constant, copying the given data.  The number
    /// of elements is determined by the array type.  The name is optional; it
    /// is copied.
    IRStringArrayConst(const std::string* first, const IRArrayType* type,
                       const char* name="") :
        IRConst(kIRStringArrayConst, type, name),
        mStrings(first, first + type->GetLength())
    {
        assert(type->GetLength() >= 0 && "Expected fixed-length array type");
    }
    
    /// Check if a value is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRValue* val) { 
        return val->GetKind() == kIRStringArrayConst;
    }

private:
    std::vector<std::string> mStrings;
};

/// Print a string array constant.
std::ostream& operator<<(std::ostream& out, const IRStringArrayConst& constant);

#endif // ndef IR_STRING_ARRAY_CONST_H
