// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef IR_ARRAY_TYPE_H
#define IR_ARRAY_TYPE_H

#include "ir/IRType.h"
#include <iosfwd>

/// IR array type.
class IRArrayType : public IRType {
public:
    /// Get the element type.
    const IRType* GetElementType() const { return mElementType; }

    /// Get the length (-1 if unspecified).
    int GetLength() const { return mLength; }

    /// Check if a type is an instance of this class. Required by IRCast().
    static bool IsInstance(const IRType* type) { 
        return type->GetKind() == kIRArrayTy;
    }

    /// Ordering for a set of array types (needed by IRTypes factory).
    bool operator<(const IRArrayType& ty) const;

protected:
    /// Array types are constructed only by the IRTypes factory.
    friend class IRTypes;

    /// Construct an array type from its element type and length.
    /// An unspecified length is represented by -1.  Shader and 
    /// method parameters can have fixed but unspecified length.
    /// Resizable arrays also have unspecified length.
    IRArrayType(const IRType* elementType, int length) :
        IRType(kIRArrayTy),
        mElementType(elementType),
        mLength(length)
    {
    }

private:
    const IRType* mElementType;
    int mLength;
};

/// Print an array type.
std::ostream& operator<<(std::ostream& out, const IRArrayType& ty);

#endif // ndef IR_ARRAY_TYPE_H
