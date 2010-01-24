// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRStringConst.h"
#include <iostream>

std::ostream& 
operator<<(std::ostream& out, const IRStringConst& constant)
{
    // TODO: re-escape control characters.
    return out << '"' << constant.Get() << '"';
}
