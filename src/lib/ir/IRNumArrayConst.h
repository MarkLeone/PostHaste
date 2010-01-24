// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_NUM_ARRAY_CONST_H
#define IR_NUM_ARRAY_CONST_H

#include "ir/IRArrayType.h"
#include "ir/IRConst.h"
#include <assert.h>
#include <iosfwd>
#include <vector>

// IR for a numeric array constant.
class IRNumArrayConst : public IRConst {
public:
    /// Get the data pointer.
    const float* GetData() const { return &mData[0]; }

    /// Get the array length.
    unsigned int GetLength() const { return mLength; }

    /// Get the data pointer for the ith element.
    const float* GetElement(unsigned int i) const
    { 
        return &mData[i * mStride];
    }

    /// Construct numeric array constant, copying the given data.  The size of
    /// the data is determined by its type.  The name is optional; it is
    /// copied.
    IRNumArrayConst(const float* data, const IRArrayType* type,
                    const char* name="") :
        IRConst(kIRNumArrayConst, type, name),
        mData(data, data + type->GetSize()),
        mLength(static_cast<unsigned int>(type->GetLength())),
        mStride(type->GetElementType()->GetSize())
    {
        assert(type->GetLength() >= 0 && "Expected fixed-length array type");
    }
    
    /// Check if a value is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRValue* val) { 
        return val->GetKind() == kIRNumArrayConst;
    }

private:
    std::vector<float> mData;
    unsigned int mLength;
    unsigned int mStride;
};

/// Print a numeric array constant.
std::ostream& operator<<(std::ostream& out, const IRNumArrayConst& constant);

#endif // ndef IR_NUM_ARRAY_CONST_H
