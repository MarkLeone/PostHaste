// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_NUM_CONST_H
#define IR_NUM_CONST_H

#include "ir/IRConst.h"
#include "ir/IRTypes.h"
#include <vector>

/// Write the given numeric data with the specified type.
void IRWriteData(const float* data, const IRType* type, std::ostream& out);

/// Abstract base class for a numeric constant (e.g. float, point, etc.)
/// or an array of numeric constants.
class IRNumConst : public IRConst {
public:
    /// Get the data pointer.
    const float* GetData() const { return &mData[0]; }

    /// Check whether this is a float constant.
    bool IsFloat() const { return GetType()->IsFloat(); }

    /// Get the value if this is a float constant (otherwise invalid).
    float GetFloat() const
    {
        assert(IsFloat() && "Float constant expected");
        return mData[0];
    }

    /// Construct numeric constant, copying the given data.  The size of the
    /// data is determined by its type.  The name is optional; it is copied.
    IRNumConst(const float* data, const IRType* type, const char* name="") :
        IRConst(kIRNumConst, type, name),
        mData(data, data + type->GetSize())
    {
    }

    /// Construct a float constant.  The name is optional; it is copied.
    IRNumConst(float value, const char* name="") :
        IRConst(kIRNumConst, IRTypes::GetFloatTy(), name)
    {
        mData.push_back(value);
    }
    
    /// Check if a value is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRValue* val) { 
        return val->GetKind() == kIRNumConst;
    }

protected:
    std::vector<float> mData;
};

/// Print a numeric constant.
std::ostream& operator<<(std::ostream& out, const IRNumConst& constant);

#endif // ndef IR_NUM_CONST_H
