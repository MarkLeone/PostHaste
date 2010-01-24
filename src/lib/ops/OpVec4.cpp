// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ops/OpVec4.h"
#include <iostream>

std::ostream& operator<<(std::ostream& out, const OpVec4& v) {
    return out << '(' << v[0] << ' ' << v[1] 
               << ' ' << v[2] << ' ' << v[3] << ')';
}
