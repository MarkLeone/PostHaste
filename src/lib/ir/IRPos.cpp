// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRPos.h"
#include "ir/IRStringConst.h"
#include <iostream>

std::ostream&
operator<<(std::ostream& out, const IRPos& pos)
{
    const char* file = pos.GetFile();
    const char* separator = "";
    if (file != NULL && *file != '\0') {
        out << file;
        separator = ":";
    }
    unsigned int line = pos.GetLine();
    if (line != 0) {
        out << separator << line;
    }        
    return out;
}
