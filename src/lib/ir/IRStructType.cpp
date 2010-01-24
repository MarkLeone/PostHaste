// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRStructType.h"
#include <iostream>

// Ordering for a set of struct types (needed by IRTypes factory).
bool 
IRStructType::operator<(const IRStructType& ty) const
{
    return mName < ty.mName ||
        mTypes < ty.mTypes ||
        mDetails < ty.mDetails ||
        mNames < ty.mNames;
}

/// Print a struct type (the entire type, not just the name).
void 
IRStructType::Dump(std::ostream& out) const
{
    out << "struct " << mName << " {\n";
    for (size_t i = 0; i < GetNumMembers(); ++i) {
        out << "    " << IRDetailToString(mDetails[i]) << " "
            << *mTypes[i] << " " << mNames[i] << ";\n";
    }
    out << "}\n";
}

std::ostream& 
operator<<(std::ostream& out, const IRStructType& ty)
{
    return out << ty.GetName();
}
