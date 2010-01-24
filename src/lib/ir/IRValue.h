// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_VALUE_H
#define IR_VALUE_H

#include "ir/IRDetail.h"
#include "ir/IRTypedefs.h"
#include "ir/IRValueKind.h"
#include <iosfwd>
class IRType;

/// Abstract base class of IR values, including constants, variables, and
/// intructions.
class IRValue {
public:
    /// Construct an IR value of the specified kind with the given type and
    /// detail.
    IRValue(IRValueKind kind, const IRType* type, IRDetail detail) :
        mKind(kind),
        mType(type),
        mDetail(detail)
    {
    }

    /// Destructor (not sure why it can't be pure virtual).
    virtual ~IRValue();

    /// Get the type of this value.
    const IRType* GetType() const { return mType; }

    /// Get the detail of this value.
    IRDetail GetDetail() const { return mDetail; }
    
    /// What kind of value is this?
    IRValueKind GetKind() const { return mKind; }

    /// Returns true if this value is a constant.
    bool IsConst() const;

    /// Returns true if this value is a variable.
    bool IsVar() const;

    /// Returns true if this value is an instruction.
    bool IsInst() const;

    /// Check if a value is an instance of this class. Required by UtCast().
    static bool IsInstance(const IRValue* val) { 
        return true;  // Every IRValue is an instance of this class.
    }

private:
    IRValueKind mKind;
    const IRType* mType;
    IRDetail mDetail;
};

/// Print an IR value.
std::ostream& operator<<(std::ostream& out, const IRValue& val);

/// Print a comma-separated list of values.
void IRWriteValues(std::ostream& out, const IRValues& vals);

#endif // ndef IR_VALUE_H
