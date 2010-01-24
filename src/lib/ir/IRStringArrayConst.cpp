// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRStringArrayConst.h"
#include <iostream>

std::ostream& 
operator<<(std::ostream& out, const IRStringArrayConst& strs)
{
    out << "{";
    for (unsigned int i = 0; i < strs.GetLength(); ++i) {
        if (i > 0)
            out << ", ";
        // TODO: re-escape control characters in string constant.
        out << '"' << strs.GetElement(i) << '"';
    }
    out << "}";
    return out;
}
