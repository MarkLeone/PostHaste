// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRShaderParam.h"
#include "ir/IRStmt.h"
#include "ir/IRType.h"
#include <iostream>
#include <iomanip>              // for std::setw

IRShaderParam::~IRShaderParam()
{
    delete mInit;
}

void
IRShaderParam::WriteDecl(std::ostream& out, unsigned int indent) const
{
    out << std::setw(indent) << "";
    if (IsOutput())
        out << "output ";
    out << IRDetailToString(GetDetail()) << " "
        << *GetType() << " " 
        << GetShortName();
    IRStmt* init = GetInitStmt();
    assert(init != NULL && "Initializer should never be NULL");
    if (!init->IsEmpty()) {
        out << " = ";
        init->WriteInit(out, this, indent+4);
    }
}

