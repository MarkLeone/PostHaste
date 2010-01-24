// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ops/OpMatrix4.h"
#include <iostream>

std::ostream& operator<<(std::ostream& out, const OpMatrix4& m) {
    return out << '(' << m[0] << ' ' << m[1] 
               << ' ' << m[2] << ' ' << m[3] << ')';
}
