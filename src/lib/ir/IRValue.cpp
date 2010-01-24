// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#include "ir/IRValue.h"
#include "ir/IRValues.h"
#include "util/UtCast.h"
#include <iostream>

IRValue::~IRValue() { }

bool 
IRValue::IsConst() const
{
    return UtIsInstance<const IRConst*>(this);
}

bool 
IRValue::IsVar() const
{
    return UtIsInstance<const IRVar*>(this);
}

bool 
IRValue::IsInst() const
{
    return UtIsInstance<const IRInst*>(this);
}

std::ostream& 
operator<<(std::ostream& out, const IRValue& val)
{
    if (const IRConst* constant = UtCast<const IRConst*>(&val))
        return out << *constant;
    if (const IRVar* var = UtCast<const IRVar*>(&val))
        return out << *var;
    if (const IRInst* inst = UtCast<const IRInst*>(&val))
        return out << *inst;
    assert(false && "Unknown kind of IRValue");
    return out << "unknown";
}


// Print a comma-separated list of values.
void
IRWriteValues(std::ostream& out, const IRValues& vals)
{
    IRValues::const_iterator it;
    for (it = vals.begin(); it != vals.end(); ++it) {
        if (it != vals.begin())
            out << ", ";
        // If the argument is an instruction, print its result variable.
        if (IRInst* inst = UtCast<IRInst*>(*it)) {
            assert(inst->GetResult() != NULL && 
                   "Instruction with no result used as argument");
            out << *inst->GetResult();
        }
        else {
            out << **it;
        }
    }
}
