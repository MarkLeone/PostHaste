// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRArrayType.h"

#include <iostream>

// Print an array type.
std::ostream& 
operator<<(std::ostream& out, const IRArrayType& ty)
{
    if (ty.GetLength() == -1) 
        out << *ty.GetElementType() << "[]";
    else 
        out << *ty.GetElementType() << "[" << ty.GetLength() << "]";
    return out;
}

// Ordering for a set of array types (needed by IRTypes factory).
bool 
IRArrayType::operator<(const IRArrayType& ty) const
{
    bool result =  mElementType < ty.mElementType || mLength < ty.mLength;
    return result;
}
