// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRVar.h"
#include "ir/IRType.h"
#include <iostream>
#include <iomanip>              // for std::setw()

IRVar::~IRVar() { }

std::ostream& 
operator<<(std::ostream& out, const IRVar& var)
{
    out << var.GetShortName();
    return out;
}

void
IRVar::WriteDecl(std::ostream& out, unsigned int indent) const
{
    out << std::setw(indent) << ""
        << IRDetailToString(GetDetail()) << " "
        << *GetType() << " " 
        << GetShortName();
}
